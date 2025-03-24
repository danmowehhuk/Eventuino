#include <Eventuino.h>
#include <TestTool.h>
#include "EventuinoTestHelper.h"

eventuino::EventuinoTestHelper helper;

void before() {
  helper.digitalReadValue = HIGH;
  helper.didPinSetup = false;
}

struct CallbackCapture {
  uint8_t value = 0;
  uint8_t callCount = 0;
};

void testDigitalPinSourceBasic(TestInvocation* t) {
  t->setName(F("DigitalPinSource debounced triggering"));
  DigitalPinSource dps = helper.digitalPinSrc(1, 6);
  helper.doSetup(&dps);
  t->assert(helper.didPinSetup, "Setup function should have been called");
  CallbackCapture capture;
  auto onChange = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  dps.onChangeState = onChange;

  helper.doBouncyActivate(&dps, &capture);
  t->assert(capture.value == 6, F("Expected value = 6"));
  t->assert(capture.callCount == 1, F("Should have been debounced to one call (1)"));
  helper.doBouncyDeactivate(&dps, &capture);
  t->assert(capture.value == 6, F("Expected value = 6"));
  t->assert(capture.callCount == 2, F("Should have been debounced to one call (2)"));
}

void testButtonBasic(TestInvocation* t) {
  t->setName(F("Button press and release behaviors"));
  Button btn = helper.buttonSrc(1, 5);
  helper.doSetup(&btn);
  t->assert(helper.didPinSetup, "Setup function should have been called");
  CallbackCapture pressCapture;
  auto onPressed = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  CallbackCapture releaseCapture;
  auto onReleased = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  btn.onPressed = onPressed;
  btn.onReleased = onReleased;

  t->assert(!btn.isPressed(), F("Must start in inactive state"));
  helper.doBouncyActivate(&btn, &pressCapture);
  t->assert(btn.isPressed(), F("Should be active"));
  t->assert(pressCapture.callCount == 1, F("onPressed not called"));
  t->assert(pressCapture.value == 5, F("Expected value = 5"));
  helper.doBouncyDeactivate(&btn, &releaseCapture);
  t->assert(!btn.isPressed(), F("Should be inactive"));
  t->assert(releaseCapture.callCount == 1, F("onReleased not called"));
  t->assert(releaseCapture.value == 5, F("Expected value = 5"));
}

void testButtonLongPress(TestInvocation* t) {
  t->setName(F("Button long press behaviors"));
  Button btn = helper.buttonSrc(1, 3);
  CallbackCapture longPressCapture;
  auto onPressed = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  btn.enableRepeat(true);
  btn.onPressed = onPressed;
  btn.onLongPress = onPressed;

  t->assert(!btn.isPressed(), F("Must start in inactive state"));
  helper.doBouncyActivate(&btn, &longPressCapture);
  t->assert(btn.isPressed(), F("Should be active"));
  t->assert(!btn.isLongPressed(), F("Should not be long pressed yet"));
  t->assert(longPressCapture.callCount == 1, F("onPressed should have been called once"));
  t->assert(longPressCapture.value == 3, F("Expected value = 3"));
  delay(50); // long press delay
  helper.doPoll(&btn, &longPressCapture);
  t->assert(btn.isLongPressed(), F("Should be long pressed"));
  t->assert(longPressCapture.callCount == 2, F("2 calls expected"));
  t->assert(longPressCapture.value == 3, F("Expected value = 3"));
  delay(12); // repeat delay
  helper.doPoll(&btn, &longPressCapture);
  t->assert(btn.isLongPressed(), F("Should still be long pressed"));
  t->assert(longPressCapture.callCount == 3, F("3 calls expected"));
  delay(12); // repeat delay
  helper.doPoll(&btn, &longPressCapture);
  t->assert(longPressCapture.callCount == 4, F("4 calls expected"));
}

void testToggle(TestInvocation* t) {
  t->setName(F("Toggle standard behaviors"));
  Toggle tog = helper.toggleSrc(1, 2);
  helper.doSetup(&tog);
  t->assert(helper.didPinSetup, "Setup function should have been called");
  CallbackCapture flipCapture;
  auto onFlip = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  tog.onFlip = onFlip;
  tog.onActivate = onFlip;
  tog.onDeactivate = onFlip;

  t->assert(!tog.isActivated(), F("Must start in inactive state"));
  helper.doBouncyActivate(&tog, &flipCapture);
  t->assert(tog.isActivated(), F("Should be active"));
  t->assert(flipCapture.callCount == 2, F("onFlip should have been called twice"));
  t->assert(flipCapture.value == 2, F("Expected value = 2"));
  helper.doBouncyDeactivate(&tog, &flipCapture);
  t->assert(!tog.isActivated(), F("Should be inactive"));
  t->assert(flipCapture.callCount == 4, F("onFlip should have been called 4 times"));
  t->assert(flipCapture.value == 2, F("Expected value = 2"));
}

void testTimer(TestInvocation* t) {
  t->setName(F("Timer standard behaviors"));
  Timer14Bit tmr = helper.timerSrc(9);
  CallbackCapture capture;
  auto onExpire = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  tmr.onExpire = onExpire;

  tmr.start(20);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.callCount == 0, F("onExpired should not have been called yet (1)"));
  delay(22);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.value == 9, F("Expected value = 9"));
  t->assert(capture.callCount == 1, F("onExpired should have been called once"));
  delay(22);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.callCount == 1, F("onExpired called by expired timer"));
  tmr.start(20);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.callCount == 1, F("onExpired should not have been called yet (2)"));
  tmr.cancel();
  delay(22);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.callCount == 1, F("onExpired called by cancelled timer"));
}

void testIntervalTimer(TestInvocation* t) {
  t->setName(F("IntervalTimer standard behaviors"));
  IntervalTimer14Bit tmr = helper.intervalTimerSrc(11);
  CallbackCapture capture;
  auto onExpire = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  tmr.onExpire = onExpire;

  tmr.start(20);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.callCount == 0, F("onExpired should not have been called yet"));
  delay(22);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.value == 11, F("Expected value = 11"));
  t->assert(capture.callCount == 1, F("onExpired should have been called once"));
  delay(22);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.callCount == 2, F("onExpired should have been called 2x"));
  delay(22);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.callCount == 3, F("onExpired should have been called 3x"));
  tmr.cancel();
  delay(22);
  helper.doPoll(&tmr, &capture);
  t->assert(capture.callCount == 3, F("onExpired called by cancelled timer"));
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  TestFunction tests[] = {

    testDigitalPinSourceBasic,
    testButtonBasic,
    testButtonLongPress,
    testToggle,
    testTimer,
    testIntervalTimer

  };

  runTestSuiteShowMem(tests, before, nullptr);
}

void loop() {}
