/*

  eventuino::Button.h

  Appropriate for use with any kind of momentary switch; i.e. a button.

  Invokes callback functions for:
  - onPressed
  - onLongPress
  - onReleased

  Uses 21 bytes of global variable space per button.

  NOTE: The button pin is expected to be HIGH when the button is not pressed.

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef eventuino_Button_h
#define eventuino_Button_h

#include "DigitalPinSource.h"

using namespace eventuino;

class Button: public DigitalPinSource {

  public:

    /*
     * Button constructor using Arduino-provided functions for
     * pinMode(...), digitalWrite(...) and digitalRead(...)
     *
     * pinNumber - The pin assigned to this button
     * value     - The value passed to the event callback functions
     */
    Button(uint8_t pinNumber, uint8_t value): DigitalPinSource(pinNumber, value) {};

    /*
     * Button constructor using custom callback functions to initialize
     * the pin and perform a digital read. These are necessary
     * when interacting with a button through a port expander such 
     * as the MCP23017 or for more complex arrays of buttons.
     */
    Button(uint8_t pinNumber, uint8_t value, 
        pinSetupCallback_t setupCallback, digitalReadCallback_t readCallback):
        DigitalPinSource(pinNumber, value, setupCallback, readCallback) {};

    eventuinoCallback_t onPressed = 0;
    eventuinoCallback_t onReleased = 0;
    eventuinoCallback_t onLongPress = 0;
    void clearCallbacks();

    bool isPressed();
    bool isLongPressed();

  private:
    // disable default constructor
    Button();

    // disable copy constructor since it would reference same pin
    Button(Button &t);

    // required by DigitalPinSource
    void onChange(uint8_t value, void* state = nullptr) override;

    // overriding default from DigitalPingSource
    void onLongHold(uint8_t value, void* state = nullptr) override;

};

#endif
