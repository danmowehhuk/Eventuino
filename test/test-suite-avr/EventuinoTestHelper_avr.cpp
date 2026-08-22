// Bare-metal AVR implementation of eventuino::EventuinoTestHelper -
// mirrors ../test-suite/EventuinoTestHelper.cpp exactly except for the
// substitutions noted at each call site below. EventuinoTestHelper.h
// (unmodified, shared with the Arduino-branch test suite) declares the
// class this implements.

#include "../test-suite/EventuinoTestHelper.h"
#include <util/delay.h>
#include "../../src/hal/EventuinoHal.h"

uint8_t EventuinoTestHelper::digitalReadValue = EventuinoHal::HIGH_STATE; // inactive

bool EventuinoTestHelper::didPinSetup = false;

void EventuinoTestHelper::helperPinSetup(uint8_t) {
  didPinSetup = true;
}

uint8_t EventuinoTestHelper::helperDigitalRead(uint8_t) {
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

void EventuinoTestHelper::doPoll(EventSource* es, void* state) {
  setEventSource(es);
  _evt.poll(state);
  clearEventSource();
}

void EventuinoTestHelper::doBouncyActivate(DigitalPinSource* dps, void* state) {
  setEventSource(dps);
  digitalReadValue = EventuinoHal::LOW_STATE;
  _evt.poll(state);
  digitalReadValue = EventuinoHal::HIGH_STATE;
  _evt.poll(state);
  digitalReadValue = EventuinoHal::LOW_STATE;
  _evt.poll(state);
  _delay_ms(15);
  _evt.poll(state);
  clearEventSource();
}

void EventuinoTestHelper::doBouncyDeactivate(DigitalPinSource* dps, void* state) {
  setEventSource(dps);
  digitalReadValue = EventuinoHal::HIGH_STATE;
  _evt.poll(state);
  digitalReadValue = EventuinoHal::LOW_STATE;
  _evt.poll(state);
  digitalReadValue = EventuinoHal::HIGH_STATE;
  _evt.poll(state);
  _delay_ms(15);
  _evt.poll(state);
  clearEventSource();
}
