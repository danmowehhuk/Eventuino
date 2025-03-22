#include <Eventuino.h>
#include <TestTool.h>
#include "EventuinoTestHelper.h"

EventuinoTestHelper helper;

void before() {
  helper.digitalReadValue = HIGH;
  helper.didPinSetup = false;
}

struct CallbackCapture {
  uint8_t value = 0;
  uint8_t callCount = 0;
};

void testDigitalPinSource(TestInvocation* t) {
  t->setName(F("DigitalPinSource debounced triggering"));
  DigitalPinSource dps = helper.digitalPinSrc(1, 6);
  CallbackCapture capture;
  auto onChange = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  dps.onChangeState = onChange;
  helper.doBouncyActivate(&dps, &capture);
  t->assert(capture.value == 6, F("Expected value = 6"));
  t->assert(capture.callCount == 1, F("Expected callCount = 1"));
}


void setup() {
  Serial.begin(9600);
  while (!Serial);

  TestFunction tests[] = {

    testDigitalPinSource

  };

  runTestSuiteShowMem(tests, before, nullptr);
}

void loop() {}
