#include "EventuinoHal.h"

#ifdef NO_ARDUINO
#include <BareMetalHAL.h>

namespace EventuinoHal {

const uint8_t HIGH_STATE = BareMetalHAL::HIGH;
const uint8_t LOW_STATE = BareMetalHAL::LOW;

void pinModeInputPullup(uint8_t pin) {
  BareMetalHAL::pinMode(pin, BareMetalHAL::INPUT_PULLUP);
}

uint8_t digitalReadPin(uint8_t pin) {
  return BareMetalHAL::digitalRead(pin);
}

unsigned long millis() {
  return BareMetalHAL::millis();
}

void println(const char* message) {
  BareMetalHAL::Uart0::println(message);
}

}  // namespace EventuinoHal

#endif  // NO_ARDUINO
