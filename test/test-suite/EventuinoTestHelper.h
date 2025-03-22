#ifndef __test_EventuinoTestHelper_h
#define __test_EventuinoTestHelper_h


#include <Eventuino.h>
#include "eventuino/DigitalPinSource.h"

class EventuinoTestHelper {

  public:
    EventuinoTestHelper() {};
    static void helperPinSetup(uint8_t pinNumber);
    static uint8_t helperDigitalRead(uint8_t pinNumber);
    static uint8_t digitalReadValue;
    static bool didPinSetup;

    DigitalPinSource digitalPinSrc(uint8_t pinNumber, uint8_t value);
    void doBouncyActivate(DigitalPinSource* dps, void* state = nullptr);
    void doBouncyDeactivate(DigitalPinSource* dps, void* state = nullptr);


  private:
    EventuinoTestHelper(EventuinoTestHelper &t) = delete;
    void setEventSource(EventSource* es);
    Eventuino _evt;


};


#endif