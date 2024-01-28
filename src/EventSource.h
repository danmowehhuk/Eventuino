/*

  eventuino::EventSource.h

  All event emitters handled by Eventuino must extend this class.

  Copyright (c) 2024, Dan Mowehhuk (danmowehhuk@gmail.com)
  All rights reserved.

*/

#ifndef eventuino_EventSource_h
#define eventuino_EventSource_h

#include <stdint.h>

namespace eventuino {

  class EventSource {

    public:

      /*
       * Initialize the event source, typically by setting the pinMode
       */
      virtual void setup() = 0;

      /*
       * Check the state of the event source. If any events have been
       * triggered, invoke the relevant callback function.
       */
      virtual void poll() = 0;

      /*
       * Event callback functions must use this signature, where the
       * "value" is specified in the constructor of the sub-class 
       * (Button, Toggle, etc) and may be used to identify which 
       * button, toggle, etc., triggered the event.
       */
      typedef void (*eventuinoCallback_t)(uint8_t value);

  };

}

#endif
