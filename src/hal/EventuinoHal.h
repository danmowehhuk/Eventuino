/*

  hal/EventuinoHal.h

  Thin facade over the GPIO, Timing, and Serial primitives Eventuino's
  core (src/) needs. On Arduino (the unconditional default when
  NO_ARDUINO isn't defined), delegates directly to the Arduino core.

  Off Arduino (NO_ARDUINO + HAL_xxx), every function below is declared
  but not yet defined - this is Phase 1 of the arduino-insert-hal
  migration (see .claudework/arduino-dependency-report.md), which only
  extracts the interface and verifies the Arduino branch is unchanged.
  Phase 2 fills these in against BareMetalHAL, which doesn't have GPIO
  or Timing categories yet (Serial already does, but stays a bare
  declaration here too for a clean, uniform phase boundary rather than
  special-casing the one category that happens to already exist).

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef EVENTUINO_HAL_EVENTUINOHAL_H
#define EVENTUINO_HAL_EVENTUINOHAL_H

#include <stdint.h>

#ifndef NO_ARDUINO
#include <Arduino.h>
#endif

namespace EventuinoHal {

#ifndef NO_ARDUINO

// Mirror Arduino's own HIGH/LOW exactly, so callers comparing pin state
// keep working unchanged.
const uint8_t HIGH_STATE = HIGH;
const uint8_t LOW_STATE = LOW;

// Eventuino's default pin setup only ever does pinMode(pin, INPUT_PULLUP)
// - never a generic mode - so the facade wraps that one operation
// directly instead of exposing a generic pinMode()/INPUT_PULLUP pair.
inline void pinModeInputPullup(uint8_t pin) { pinMode(pin, INPUT_PULLUP); }
inline uint8_t digitalReadPin(uint8_t pin) { return digitalRead(pin); }
inline unsigned long millis() { return ::millis(); }
inline void println(const char* message) { Serial.println(message); }

#else

extern const uint8_t HIGH_STATE;
extern const uint8_t LOW_STATE;

void pinModeInputPullup(uint8_t pin);
uint8_t digitalReadPin(uint8_t pin);
unsigned long millis();
void println(const char* message);

#endif

}  // namespace EventuinoHal

#endif
