// Bare-metal AVR port of ../button_basic/button_basic.ino - demonstrates
// Eventuino's NO_ARDUINO branch linking and running against BareMetalHAL's
// GPIO, Timing, and Dynamic Memory categories together, not just
// individually. See the "Building without Arduino" section of the
// top-level README for the full rationale.
//
// Button wiring: PL7, pulled up internally (BareMetalHAL::INPUT_PULLUP),
// grounded when pressed - change the port/bit below to match your wiring.
// Uses Button's default (2-argument) constructor, now unconditional
// since BareMetalHAL provides a real default under HAL_AVR too.
//
// Every callback uses the full eventuinoCallback_t signature
// (uint8_t, void*) - bare avr-g++ has no -fpermissive leniency for a
// 1-argument shorthand, unlike arduino-cli.

#include <util/delay.h>
#include <BareMetalHAL.h>
#include <Eventuino.h>
#include <eventuino/Button.h>

using namespace eventuino;
using namespace BareMetalHAL;

Button button(pin(Port::L, 7), 1);
Eventuino evt;

void buttonPressed(uint8_t value, void*) {
  Uart0::print("Button pressed with value=");
  Uart0::println((int)value);
}

int main() {
  Uart0::begin(9600);
  timingInit();

  button.onPressed = buttonPressed;

  evt.addEventSource(&button);
  evt.begin();

  while (true) {
    evt.poll();
  }
}
