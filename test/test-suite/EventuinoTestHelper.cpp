#include "EventuinoTestHelper.h"
#include <Arduino.h>

uint8_t EventuinoTestHelper::digitalReadValue = HIGH; // inactive

bool EventuinoTestHelper::didPinSetup = false;

void EventuinoTestHelper::helperPinSetup(uint8_t pinNumber) {
  didPinSetup = true;
}
    
uint8_t EventuinoTestHelper::helperDigitalRead(uint8_t pinNumber) {
  return digitalReadValue;
}

void EventuinoTestHelper::setEventSource(EventSource* es) {
  EventSource* evtSources[1] = { es };
  _evt.setEventSources(evtSources, 1);
}

DigitalPinSource EventuinoTestHelper::digitalPinSrc(uint8_t pinNumber, uint8_t value) {
  DigitalPinSource dps(pinNumber, value, EventuinoTestHelper::helperPinSetup, EventuinoTestHelper::helperDigitalRead);
  // make these faster for testing
  dps.setDebounceDelayMs(10);
  dps.setLongHoldDelayMs(50);
  dps.setRepeatMs(10);
  return dps;
}

void EventuinoTestHelper::doBouncyActivate(DigitalPinSource* dps, void* state = nullptr) {
  setEventSource(dps);
  digitalReadValue = LOW;
  _evt.poll(state);
  digitalReadValue = HIGH;
  _evt.poll(state);
  digitalReadValue = LOW;
  _evt.poll(state);
  delay(15);
  _evt.poll(state);
}

void EventuinoTestHelper::doBouncyDeactivate(DigitalPinSource* dps, void* state = nullptr) {
  setEventSource(dps);
  digitalReadValue = HIGH;
  _evt.poll(state);
  digitalReadValue = LOW;
  _evt.poll(state);
  digitalReadValue = HIGH;
  _evt.poll(state);
  delay(15);
  _evt.poll(state);
}
