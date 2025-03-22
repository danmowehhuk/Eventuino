#include <Arduino.h>
#include "DigitalPinSource.h"

using namespace eventuino;

// Initialize static values
uint16_t DigitalPinSource::_longHoldDelayMs = 1000;
uint8_t DigitalPinSource::_repeatMs = 200;
uint8_t DigitalPinSource::_debounceDelayMs = 75;

void _eventuinoPinSetupDefault(uint8_t pinNumber) {
  pinMode(pinNumber, INPUT_PULLUP);
}

uint8_t _eventuinoDigitalReadDefault(uint8_t pinNumber) {
  return digitalRead(pinNumber);
}

DigitalPinSource::DigitalPinSource(uint8_t pinNumber, uint8_t value):
    EventSource(), _pinNumber(pinNumber), _value(value),
    _doPinSetup(_eventuinoPinSetupDefault),
    _doDigitalRead(_eventuinoDigitalReadDefault) {};

DigitalPinSource::DigitalPinSource(uint8_t pinNumber, uint8_t value, 
      pinSetupCallback_t setupCallback, digitalReadCallback_t readCallback):
    EventSource(), _pinNumber(pinNumber), _value(value),
    _doPinSetup(setupCallback),
    _doDigitalRead(readCallback) {};

void DigitalPinSource::poll(void* state = nullptr) {
  uint16_t now = millis(); // trunc to last 16-bits (32s)
  uint8_t reading = _doDigitalRead(_pinNumber);

  if (reading != prevState()) {
    // Pin state has changed, but might be noise
    _toggleTime = now;
    setPrevState(reading);
  }

  if ((now - _toggleTime) > _debounceDelayMs) {
    // Pin state is steady, ready to check for events
    // Start by storing the new state
    uint8_t prevState = currState();
    setCurrState(reading);

    if (reading != prevState) {
      // State has changed

      if (reading == LOW && prevState == HIGH) {
        // State changed from inactive to active
        _lastRepeat = now;
        setIsActive(true);
      } else {
        // State changed from active to inactive
        _toggleTime = 0;
        _lastRepeat = 0;
        setIsActive(false);
        setIsLongHold(false);
      }
      onChange(_value, state);

    } else {
      // State is unchanged, check for long hold

      if (reading == LOW &&
          (now - _toggleTime > _longHoldDelayMs) && 
          (now - _lastRepeat > _repeatMs)) {
        // Pin has been active long enough for long hold
        // Possibly also a repeat long hold if repeat enabled

        bool isInitialLongHold = !isLongHold();
        setIsLongHold(true);
        if (isInitialLongHold || isRepeatEnabled()) {
          onLongHold(_value, state);
          _lastRepeat = now;
        } else {
          // This is repeat pass, and repeat is disabled - do nothing
        }  
      }

    }
  }
}

bool DigitalPinSource::isRepeatEnabled() {
  return bitRead(_state, 4);
}

void DigitalPinSource::enableRepeat(bool b) {
  bitWrite(_state, 4, b);
}

bool DigitalPinSource::isActive() {
  return bitRead(_state, 3);
}

void DigitalPinSource::setIsActive(bool b) {
  bitWrite(_state, 3, b);
}

bool DigitalPinSource::isLongHold() {
  return bitRead(_state, 2);
}

void DigitalPinSource::setIsLongHold(bool b) {
  bitWrite(_state, 2, b);
}

uint8_t DigitalPinSource::currState() {
  return bitRead(_state, 1);
}

void DigitalPinSource::setCurrState(uint8_t s) {
  bitWrite(_state, 1, s);
}

uint8_t DigitalPinSource::prevState() {
  return bitRead(_state, 0);
}

void DigitalPinSource::setPrevState(uint8_t s) {
  bitWrite(_state, 0, s);
}

DigitalPinSource::DigitalPinSource(DigitalPinSource&& other) noexcept {
  onChangeState = other.onChangeState;
  _pinNumber = other._pinNumber;
  _value = other._value;
  _doDigitalRead = other._doDigitalRead;
  _doPinSetup = other._doPinSetup;
  _toggleTime = other._toggleTime;
  _lastRepeat = other._lastRepeat;
  _state = other._state;
  other.onChangeState = 0;
  other._doDigitalRead = 0;
  other._doPinSetup = 0;
}

DigitalPinSource& DigitalPinSource::operator=(DigitalPinSource&& other) noexcept {
  if (this != &other) {
    onChangeState = other.onChangeState;
    _pinNumber = other._pinNumber;
    _value = other._value;
    _doDigitalRead = other._doDigitalRead;
    _doPinSetup = other._doPinSetup;
    _toggleTime = other._toggleTime;
    _lastRepeat = other._lastRepeat;
    _state = other._state;
    other.onChangeState = 0;
    other._doDigitalRead = 0;
    other._doPinSetup = 0;
  }
  return *this;
}


