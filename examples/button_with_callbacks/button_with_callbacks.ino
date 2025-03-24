/*

 Using callbacks in the Eventuino_Button constructor allows you to
 setup, read and write pins without using the built-in Arduino
 functions. This is necessary when using a port expander such as
 the MCP23017.

*/

#include <Eventuino.h>
#include <eventuino/Button.h>

using namespace eventuino;

#define BUTTON_PIN 5
#define BUTTON_VALUE 1

void setupMyButton(uint8_t pinNumber) {

  // Here's a trick for encoding the pins of up to 8 port-expanders
  // into a single 8-bit pin number:
  // - If the most significant bit is 1, this is a port expander,
  //   otherwise, it's a regular Arduino pin. No Arduino has more
  //   than 127 I/O pins, so no worries there.
  // - The next 3 most significant bits are the port-expander
  //   address. You can only have 8 anyway, so this fits perfectly.
  // - The last 4 bits are the pin number on the port expander. 
  //   The MCP23017 has 16 pins, so this again fits perfectly.

  if (bitRead(pinNumber, 7)) {
    // It's a port expander pin
    // 
    // Figure out which port-expander from the next 3 bits...
    // 
    // Only take the last 4 bits for the pinNumber...
    // pinNumber = pinNumber & 16;
    // myPortExpanderN.pinMode(pinNumber, INPUT_PULLUP);
  } else {
    pinMode(pinNumber, INPUT_PULLUP);
  }
}

uint8_t readMyButton(uint8_t pinNumber) {
  // Same logic as above to handle port-expanders...
  if (bitRead(pinNumber, 7)) {
    // it's a port-expander, handle as described above.
    // ...
    // return myPortExpander.digitalRead(pinNumber & 16);
  } else {
    return digitalRead(pinNumber);
  }
}

Button button(BUTTON_PIN, BUTTON_VALUE, setupMyButton, readMyButton); 
Eventuino evt;

void buttonPressed(uint8_t value) {
  Serial.print("Button pressed with value=");
  Serial.println(value);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Callbacks can be changed at any time and take effect with 
  // the next poll
  button.onPressed=buttonPressed;

  evt.addEventSource(&button);
  evt.begin();
}

void loop() {
  evt.poll();
}
