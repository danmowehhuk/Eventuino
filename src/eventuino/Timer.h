/*

  eventuino::Timer.h

  Invokes a callback when at least N milliseconds have passed.
  No pins are required. Timers can be reused after they have 
  expired, or after calling cancel(). IntervalTimers automatically
  repeat. They cannot avoid latency, but they will not drift. 
  This requires more memory than a regular Timer.

  Invokes callback functions for:
  - onExpire

  Use one of the following timer classes:
  - Timer14Bit: 9 bytes, 16s max duration
  - Timer30Bit: 11 bytes, 1 million seconds max duration (12 days)
  - IntervalTimer14Bit: 15 bytes, 16s max duration
  - IntervalTimer30Bit: 19 bytes, 1 million seconds max duration (12 days)

  Timers are robust to Arduino's millis() rolling over to 0 after 50 days.


  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef eventuino_Timer_h
#define eventuino_Timer_h

#include <Arduino.h>
#include "EventSource.h"

using namespace eventuino;

/* 
 * Do not use the Timer class directly. Instead, use Timer14Bit or
 * Timer30Bit depending on desired memory footprint.
 *
 * Templata params:
 *   U - an unsigned int type; e.g. uint8_t, uint16_t, uint32_t
 *   T - the signed version of the same int type as U
 *   S - the number of bits in U
 */
template<class U, class T, uint8_t S> class Timer: public EventSource {

  public:
    // disable default constructor
    Timer() = delete;

    /*
     * value - The value passed to the event callback functions
     */
    Timer(uint8_t value): _value(value) {};

    // no pins to set up
    void setup() override {};

    // required by EventSource
    void poll(void* state = nullptr) override {
      if (!isActive()) return;
      if (onExpire == 0) {
        cancel();
        return;
      }
      if (isExpired()) {
        reset();
        onExpire(_value, state);
      }
    };

    /*
     * Start the timer. Calling this again will restart the timer.
     *
     * duration  - The minimum number of milliseconds before onExpire is called
     */
    void start(U duration) {
      if (onExpire == 0) return;
      uint32_t startTime = millis();
      setInterval(startTime, duration);
      updateExpiration(startTime, duration);
      setActive(true);
    };

    /*
     * Cancel the timer
     */
    void cancel() {
      _state = 0; // isActive now false
      setInterval(0, 0);
    };

    eventuinoCallback_t onExpire = 0;

    // Disable copying
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    // Allow moving
    Timer(Timer&& other) noexcept {
      onExpire = other.onExpire;
      _value = other._value;
      _state = other._state;
      other.onExpire = 0;
      other._value = 0;
      other._state = 0;
    };
    Timer& operator=(Timer&& other) noexcept {
      if (this != &other) {
        onExpire = other.onExpire;
        _value = other._value;
        _state = other._state;
        other.onExpire = 0;
        other._value = 0;
        other._state = 0;
      }
      return *this;
    }

  protected:
    uint8_t _value;

    // bits: isActive | isOverflow | expirationTime (all remaining bits)
    U _state = 0;

    virtual void setInterval(uint32_t startTime, U duration) {};
    virtual void reset() { cancel(); };

    void updateExpiration(uint32_t startTime, U duration) {
      U expires = startTime + duration;
      U mask = ~((U)3 << (S - 2)); // 0b00111111...(remaining bits)
      _state = expires & mask;
      if ((1 & bitRead(startTime, S - 3)) & !bitRead(expires, S - 3)) {
        setOverflow(true);
      }
    };

    bool isExpired() {
      bool expired = false;
      U mask = ~((U)3 << (S - 2));
      uint32_t now = millis();
      U t = now & mask;
      U expires = _state & mask;

      if (isOverflow() && !bitRead(now, S - 3)) {
        // "now" is in the lower half of U's range
        // As long as there's not seconds of latency,
        // this means it rolled over to positive digits
        setOverflow(false);
      }

      if (isOverflow()) {
        t |= (U)3 << (S - 2);
        if ((T)t >= (T)expires) expired = true;
      } else {
        if (t >= expires) expired = true;
      }

      return expired;
    };

    void setActive(bool b)  {
      bitWrite(_state, S - 1, b ? 1 : 0);
    };
    bool isActive() {
      return (bitRead(_state, S - 1) == 1) ? true : false;
    };
    void setOverflow(bool b) {
      bitWrite(_state, S - 2, b ? 1 : 0);
    };
    bool isOverflow() {
      return (bitRead(_state, S - 2) == 1) ? true : false;
    };

    // For derived class move constructors/operators
    template<typename M>
    M&& move(M& obj) {
      return static_cast<M&&>(obj);
    }

};

/*
 * Similar to the Timer class, except that an IntervalTimer automatically restarts
 * after it expires, and ensures that the interval doesn't drift because of
 * latency in executing the onExpire callback. For example, if the interval is 
 * 100ms and the timer is started at 0ms, onExpire will be called after N*100ms
 * without drift. To stop this timer, cancel() must be called explicitly.
 * 
 * Template parameters are the same as Timer. Do not use IntervalTimer directly,
 * rather use IntervalTimer14Bit or IntervalTimer30Bit.
 */
template<class U, class T, uint8_t S> class IntervalTimer: public Timer<U, T, S> {

  public:
    IntervalTimer() = delete;

    /*
     * value - The value passed to the event callback functions
     */
    IntervalTimer(uint8_t value): Timer<U, T, S>(value) {};

    // Disable copying
    IntervalTimer(const IntervalTimer&) = delete;
    IntervalTimer& operator=(const IntervalTimer&) = delete;

    // Allow moving
    IntervalTimer(IntervalTimer&& other) noexcept: Timer<U, T, S>(move(other)) {
      _interval = other._interval;
      _prev = other._prev;
      other._interval = 0;
      other._prev = 0;
    };
    IntervalTimer& operator=(IntervalTimer&& other) noexcept {
      if (this != &other) {
        Timer<U, T, S>::operator=(move(other));
        _interval = other._interval;
        _prev = other._prev;
        other._interval = 0;
        other._prev = 0;
      }
      return *this;
    };

  private:
    U _interval;
    uint32_t _prev;

    void setInterval(uint32_t startTime, U duration) {
      _prev = startTime + duration;
      _interval = duration;
    };

    void reset() {
      updateExpiration(_prev, _interval);
      _prev += _interval;
      setActive(true);
    };

}; 

// 16s max duration
class Timer14Bit: public Timer<uint16_t, int16_t, 16> {
  public:
    Timer14Bit(uint8_t value): Timer(value) {};
};

// 12-day max duration
class Timer30Bit: public Timer<uint32_t, int32_t, 32> {
  public:
    Timer30Bit(uint8_t value): Timer(value) {};
};

// 16s max duration
class IntervalTimer14Bit: public IntervalTimer<uint16_t, int16_t, 16> {
  public:
    IntervalTimer14Bit(uint8_t value): IntervalTimer(value) {};
};

// 12-day max duration
class IntervalTimer30Bit: public IntervalTimer<uint32_t, int32_t, 32> {
  public:
    IntervalTimer30Bit(uint8_t value): IntervalTimer(value) {};
};


#endif
