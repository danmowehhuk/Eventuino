/*

  eventuino::Eventuino.h

  C++ Event Framework for Arduino for buttons, switches, potentiometers and other human interface
  devices.

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef Eventuino_h
#define Eventuino_h

#include "EventSource.h"
#include <stdint.h>

using namespace eventuino;

namespace eventuino {

  class Eventuino {

    private:
      Eventuino(Eventuino &t); // disable copy constructor
      EventSource* *_eventSources;
      uint8_t _eventSourceCount;

    public:
      Eventuino() {};

      void setEventSources(EventSource* *eventSources, uint8_t eventSourceCount);
    
      /*
       * Calls setup() on all the EventSources. Typically used to set the source's pinMode.
       */
      void begin();

      /*
       * Calls poll() on all the EventSources. This can be called from the Arduino loop()
       * function, or in an interrupt function.
       */
      void poll();

  };
}

#endif
