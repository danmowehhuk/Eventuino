/*

  eventuino::Timer.h

  Invokes a callback when at least N milliseconds have passed.
  No pins are required. Timers can be reused after they have 
  expired, or after calling cancel()

  Invokes callback functions for:
  - onExpire

  Note that Arduino's millis() value rolls over to 0 after 50 days.

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef eventuino_Timer_h
#define eventuino_Timer_h

#include "EventSource.h"

using namespace eventuino;

class Timer: public EventSource {

  public:
    /*
     * value     - The value passed to the event callback functions
     */
    Timer(uint8_t value): _value(value) {};

    void setup() {}; // no pins to set up
    void poll(); // required by EventSource

    /*
     * Start the timer. Calling this again will restart the timer.
     *
     * duration  - The minimum number of milliseconds before onExpire is called
     */
    void start(uint32_t duration);

    /*
     * Cancel the timer
     */
    void cancel();

    eventuinoCallback_t onExpire = 0;


  private:
    bool _isActive = false;
    uint8_t _value;
    uint32_t _duration = 0;
    uint32_t _startTime = 0;

};


#endif
