/*

  hal/EventuinoHal.h

  This namespace consolidates the GPIO, Timing, and Serial calls
  Eventuino's core (src/) needs so they can be redirected to
  BareMetalHAL when building with -DNO_ARDUINO.

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
