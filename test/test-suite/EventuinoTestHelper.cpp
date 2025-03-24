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
  if (_evt._eventSources) delete[] _evt._eventSources;
  EventSource** evtSources = new EventSource*[1] { es };
  _evt._eventSources = evtSources;
  _evt._eventSourceCount = 1;
} 

void EventuinoTestHelper::clearEventSource() {
  if (_evt._eventSources) delete[] _evt._eventSources;
  _evt._eventSources = nullptr;
  _evt._eventSourceCount = 0;
}

DigitalPinSource EventuinoTestHelper::digitalPinSrc(uint8_t pinNumber, uint8_t value) {
  DigitalPinSource dps(pinNumber, value, EventuinoTestHelper::helperPinSetup, EventuinoTestHelper::helperDigitalRead);
  // make these faster for testing
  dps.setDebounceDelayMs(10);
  dps.setLongHoldDelayMs(50);
  dps.setRepeatMs(10);
  return dps;
}

Button EventuinoTestHelper::buttonSrc(uint8_t pinNumber, uint8_t value) {
  Button b(pinNumber, value, EventuinoTestHelper::helperPinSetup, EventuinoTestHelper::helperDigitalRead);
  b.setDebounceDelayMs(10);
  b.setLongHoldDelayMs(50);
  b.setRepeatMs(10);
  return b;
}

Toggle EventuinoTestHelper::toggleSrc(uint8_t pinNumber, uint8_t value) {
  Toggle t(pinNumber, value, EventuinoTestHelper::helperPinSetup, EventuinoTestHelper::helperDigitalRead);
  return t;
}

Timer14Bit EventuinoTestHelper::timerSrc(uint8_t value) {
  Timer14Bit t(value);
  return t;  
}

IntervalTimer14Bit EventuinoTestHelper::intervalTimerSrc(uint8_t value) {
  IntervalTimer14Bit t(value);
  return t;  
}

void EventuinoTestHelper::doSetup(EventSource* es) {
  setEventSource(es);
  _evt.begin();
  clearEventSource();
}

void EventuinoTestHelper::doPoll(EventSource* es, void* state = nullptr) {
  setEventSource(es);
  _evt.poll(state);
  clearEventSource();
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
  clearEventSource();
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
  clearEventSource();
}
