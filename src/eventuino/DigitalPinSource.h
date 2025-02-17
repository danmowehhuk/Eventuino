/*

  Eventuino_DigitalPinSource.h

  Base class for EventSources with digital (on or off) states; for example,
  buttons and toggle switches. Anything that could drop the voltage of a
  digital pin to ground would also work (a sensor connected to a transistor
  perhaps).

  This class takes care of all debouncing, so subclasses can override the
  onChange(uint8_t) and onLongHold(uint8_t) without worrying about noise
  or needing to deal with timers.

  onChange(uint8_t) calls the onChangeState callback by default.
  The onLongHold(uint8_t) method provides an empty default implementation.

  NOTE: The "active" state means the pin is reading LOW

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef eventuino_DigitalPinSource_h
#define eventuino_DigitalPinSource_h

#include "../EventSource.h"

using namespace eventuino;

namespace eventuino {

  class DigitalPinSource: public EventSource {

    public:
      /*
       * Constructor using Arduino-provided functions for
       * pinMode(...) and digitalRead(...)
       *
       * pinNumber - A digital pin
       * value     - The value passed to the event callback functions
       */
      DigitalPinSource(uint8_t pinNumber, uint8_t value);

      /*
       * Constructor using custom callback functions to initialize
       * the pin and perform a digital read. These are necessary
       * when interacting with a pin through a port expander such 
       * as the MCP23017.
       */
      typedef uint8_t (*digitalReadCallback_t)(uint8_t pinNumber);
      typedef void (*pinSetupCallback_t)(uint8_t pinNumber);
      DigitalPinSource(uint8_t pinNumber, uint8_t value, 
          pinSetupCallback_t setupCallback, digitalReadCallback_t readCallback);

      uint8_t getValue() {
        return _value;
      }

      // Standard implementation for all digital pins.
      void setup() override {
        _doPinSetup(_pinNumber);
      }

      void poll(void* state = nullptr) override;

      /*
       * Default callback used by onChange if not overriden by a subclass
       */
      eventuinoCallback_t onChangeState = 0;

      /*
       * Call the onLongHold method repeatedly after an initial delay.
       * This is disabled by default.
       */ 
      void enableRepeat(bool b);

      /*
       * Update debounce delay, long hold delay and repeat delay for 
       * ALL digital pin sources.
       */
      static void setDebounceDelayMs(uint8_t delay) {
        _debounceDelayMs = delay;
      }
      static void setLongHoldDelayMs(uint16_t delay) {
        _longHoldDelayMs = delay;
      }
      static void setRepeatMs(uint8_t repeat) {
        _repeatMs = repeat;
      }


    protected:
      // Returns true when the pin is LOW
      bool isActive();

      // Returns true if the pin has been LOW for more than some delay
      bool isLongHold(); 

      // Called when the pin state changes (debounced). Default implementation
      // calls DigitalPinSource.onChangeState.
      virtual void onChange(uint8_t value, void* state = nullptr) {
        if (onChangeState != 0) {
          onChangeState(value, state);
        }
      };

      // Called when the pin has been active for more than some delay, and
      // possibly repeated if repeat is enabled
      virtual void onLongHold(uint8_t value, void* state = nullptr) {};

    private:
      DigitalPinSource(); // disable default constructor

      // Disable copy constructor since it would refer to the same pin
      DigitalPinSource(DigitalPinSource &t);

      uint8_t _pinNumber;
      uint8_t _value;
      digitalReadCallback_t _doDigitalRead;
      pinSetupCallback_t _doPinSetup;
      uint16_t _toggleTime = 0;
      uint16_t _lastRepeat = 0;

      // bits: 000 | enableRepeat | isActive | isLongHold | currState | prevState
      uint8_t _state = 0b00000011;

      bool isRepeatEnabled();
      uint8_t currState();
      uint8_t prevState();
      void setPrevState(uint8_t s);
      void setCurrState(uint8_t s);
      void setIsActive(bool b);
      void setIsLongHold(bool b);

      static uint16_t _longHoldDelayMs;
      static uint8_t _repeatMs;
      static uint8_t _debounceDelayMs;

  };

}

#endif
