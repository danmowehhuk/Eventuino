/*

  Eventuino.cpp

  C++ Event Framework for Arduino for buttons, switches, potentiometers and other human interface
  devices.

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#include "Eventuino.h" 
#include "EventSource.h"
#include <stdint.h>

using namespace eventuino;

__attribute__((deprecated("Use addEventSource(...) instead")))
void Eventuino::setEventSources(EventSource* *srcs, uint8_t n) {
  if (_eventSources) delete[] _eventSources;
  _eventSources = srcs;
  _eventSourceCount = n;
}

void Eventuino::addEventSource(EventSource* eventSource) {
  EventSource** newEvtSources = new EventSource*[_eventSourceCount + 1];
  for (uint8_t i = 0; i < _eventSourceCount; i++) {
    newEvtSources[i] = _eventSources[i];
  }
  newEvtSources[_eventSourceCount] = eventSource;

  delete[] _eventSources;
  _eventSources = newEvtSources;
  _eventSourceCount++;
}

void Eventuino::begin() {
  for (uint8_t i = 0; i < _eventSourceCount; i++) {
    EventSource* es = _eventSources[i];
    es->setup();
  }	
}

void Eventuino::poll(void* state = nullptr) {
  for (uint8_t i = 0; i < _eventSourceCount; i++) {
    EventSource* es = _eventSources[i];
    if (es) {
      es->poll(state);
    }
  }
}
