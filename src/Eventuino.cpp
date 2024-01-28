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

void Eventuino::setEventSources(EventSource* *srcs, uint8_t n) {
  _eventSources = srcs;
  _eventSourceCount = n;
}

void Eventuino::begin() {
  for (uint8_t i = 0; i < _eventSourceCount; i++) {
    EventSource* es = _eventSources[i];
    es->setup();
  }	
}

void Eventuino::poll() {
  for (uint8_t i = 0; i < _eventSourceCount; i++) {
    EventSource* es = _eventSources[i];
    es->poll();
  }
}
