#ifndef __test_EventuinoTestHelper_h
#define __test_EventuinoTestHelper_h


#include <Eventuino.h>
#include "eventuino/DigitalPinSource.h"
#include "eventuino/Button.h"
#include "eventuino/Toggle.h"
#include "eventuino/Timer.h"

namespace eventuino {

  class EventuinoTestHelper {

    public:
      EventuinoTestHelper() {};
      static uint8_t digitalReadValue;
      static bool didPinSetup;

      void doSetup(EventSource* es);
      void doPoll(EventSource* es, void* state = nullptr);

      void doBouncyActivate(DigitalPinSource* dps, void* state = nullptr);
      void doBouncyDeactivate(DigitalPinSource* dps, void* state = nullptr);
      DigitalPinSource digitalPinSrc(uint8_t pinNumber, uint8_t value);
      Button buttonSrc(uint8_t pinNumber, uint8_t value);
      Toggle toggleSrc(uint8_t pinNumber, uint8_t value);
      Timer14Bit timerSrc(uint8_t value);
      IntervalTimer14Bit intervalTimerSrc(uint8_t value);

    private:
      EventuinoTestHelper(EventuinoTestHelper &t) = delete;
      void setEventSource(EventSource* es);
      void clearEventSource();
      static void helperPinSetup(uint8_t pinNumber);
      static uint8_t helperDigitalRead(uint8_t pinNumber);
      Eventuino _evt;

  };
}

#endif