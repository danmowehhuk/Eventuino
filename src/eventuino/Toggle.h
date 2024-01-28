/*

  eventuino::Toggle.h

  Appropriate for use with any kind of latched switch

  Invokes callback functions for:
  - onFlip
  - onActivate
  - onDeactivate

  NOTE: The toggle pin is expected to be HIGH when the toggle is deactivated.

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef eventuino_Toggle_h
#define eventuino_Toggle_h

#include "DigitalPinSource.h"

using namespace eventuino;

class Toggle: public DigitalPinSource {

  public:

    /*
     * Toggle constructor using Arduino-provided functions for
     * pinMode(...), digitalWrite(...) and digitalRead(...)
     *
     * pinNumber - The pin assigned to this toggle switch
     * value     - The value passed to the event callback functions
     */
    Toggle(uint8_t pinNumber, uint8_t value): DigitalPinSource(pinNumber, value) {};

    /*
     * Toggle constructor using custom callback functions to initialize
     * the pin and perform a digital read. These are necessary
     * when interacting with a toggle through a port expander such 
     * as the MCP23017 or for more complex arrays of toggle switches.
     */
    Toggle(uint8_t pinNumber, uint8_t value, 
        pinSetupCallback_t setupCallback, digitalReadCallback_t readCallback):
        DigitalPinSource(pinNumber, value, setupCallback, readCallback) {};

    eventuinoCallback_t onFlip = 0;
    eventuinoCallback_t onActivate = 0;
    eventuinoCallback_t onDeactivate = 0;
    void clearCallbacks();

    bool isActivated();

  private:
    // disable default constructor
    Toggle();

    // disable copy constructor since it would reference same pin
    Toggle(Toggle &t);

    // required by DigitalPinSource
    void onChange(uint8_t value);

    // onLongHold is not supported by a Toggle, so enableRepeat is disabled
    void enableRepeat(bool b);

};

#endif
