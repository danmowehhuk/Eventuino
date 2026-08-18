// Bare-metal AVR port of ../test-suite/test-suite.ino - proves
// Eventuino's whole NO_ARDUINO chain behaves correctly under real
// (simulated) hardware, not just that it links. Uses
// EventuinoTestHelper (unmodified header, this directory's
// EventuinoTestHelper_avr.cpp implementation) exactly like the
// Arduino-branch suite does.

#include <util/delay.h>
#include <BareMetalHAL.h>
#include <Eventuino.h>
#include <TestTool.h>
#include "../test-suite/EventuinoTestHelper.h"
#include "../../src/hal/EventuinoHal.h"

eventuino::EventuinoTestHelper helper;

void before() {
  helper.digitalReadValue = EventuinoHal::HIGH_STATE;
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
  t->verify(helper.didPinSetup, "Setup function should have been called");
  CallbackCapture capture;
  auto onChange = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  dps.onChangeState = onChange;

  helper.doBouncyActivate(&dps, &capture);
  t->verify(capture.value == 6, F("Expected value = 6"));
  t->verify(capture.callCount == 1, F("Should have been debounced to one call (1)"));
  helper.doBouncyDeactivate(&dps, &capture);
  t->verify(capture.value == 6, F("Expected value = 6"));
  t->verify(capture.callCount == 2, F("Should have been debounced to one call (2)"));
}

void testButtonBasic(TestInvocation* t) {
  t->setName(F("Button press and release behaviors"));
  Button btn = helper.buttonSrc(1, 5);
  helper.doSetup(&btn);
  t->verify(helper.didPinSetup, "Setup function should have been called");
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

  t->verify(!btn.isPressed(), F("Must start in inactive state"));
  helper.doBouncyActivate(&btn, &pressCapture);
  t->verify(btn.isPressed(), F("Should be active"));
  t->verify(pressCapture.callCount == 1, F("onPressed not called"));
  t->verify(pressCapture.value == 5, F("Expected value = 5"));
  helper.doBouncyDeactivate(&btn, &releaseCapture);
  t->verify(!btn.isPressed(), F("Should be inactive"));
  t->verify(releaseCapture.callCount == 1, F("onReleased not called"));
  t->verify(releaseCapture.value == 5, F("Expected value = 5"));
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

  t->verify(!btn.isPressed(), F("Must start in inactive state"));
  helper.doBouncyActivate(&btn, &longPressCapture);
  t->verify(btn.isPressed(), F("Should be active"));
  t->verify(!btn.isLongPressed(), F("Should not be long pressed yet"));
  t->verify(longPressCapture.callCount == 1, F("onPressed should have been called once"));
  t->verify(longPressCapture.value == 3, F("Expected value = 3"));
  _delay_ms(50); // long press delay
  helper.doPoll(&btn, &longPressCapture);
  t->verify(btn.isLongPressed(), F("Should be long pressed"));
  t->verify(longPressCapture.callCount == 2, F("2 calls expected"));
  t->verify(longPressCapture.value == 3, F("Expected value = 3"));
  _delay_ms(12); // repeat delay
  helper.doPoll(&btn, &longPressCapture);
  t->verify(btn.isLongPressed(), F("Should still be long pressed"));
  t->verify(longPressCapture.callCount == 3, F("3 calls expected"));
  _delay_ms(12); // repeat delay
  helper.doPoll(&btn, &longPressCapture);
  t->verify(longPressCapture.callCount == 4, F("4 calls expected"));
}

void testToggle(TestInvocation* t) {
  t->setName(F("Toggle standard behaviors"));
  Toggle tog = helper.toggleSrc(1, 2);
  helper.doSetup(&tog);
  t->verify(helper.didPinSetup, "Setup function should have been called");
  CallbackCapture flipCapture;
  auto onFlip = [](uint8_t value, void* state = nullptr) {
    CallbackCapture* c = static_cast<CallbackCapture*>(state);
    c->value = value;
    c->callCount++;
  };
  tog.onFlip = onFlip;
  tog.onActivate = onFlip;
  tog.onDeactivate = onFlip;

  t->verify(!tog.isActivated(), F("Must start in inactive state"));
  helper.doBouncyActivate(&tog, &flipCapture);
  t->verify(tog.isActivated(), F("Should be active"));
  t->verify(flipCapture.callCount == 2, F("onFlip should have been called twice"));
  t->verify(flipCapture.value == 2, F("Expected value = 2"));
  helper.doBouncyDeactivate(&tog, &flipCapture);
  t->verify(!tog.isActivated(), F("Should be inactive"));
  t->verify(flipCapture.callCount == 4, F("onFlip should have been called 4 times"));
  t->verify(flipCapture.value == 2, F("Expected value = 2"));
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
  t->verify(capture.callCount == 0, F("onExpired should not have been called yet (1)"));
  _delay_ms(22);
  helper.doPoll(&tmr, &capture);
  t->verify(capture.value == 9, F("Expected value = 9"));
  t->verify(capture.callCount == 1, F("onExpired should have been called once"));
  _delay_ms(22);
  helper.doPoll(&tmr, &capture);
  t->verify(capture.callCount == 1, F("onExpired called by expired timer"));
  tmr.start(20);
  helper.doPoll(&tmr, &capture);
  t->verify(capture.callCount == 1, F("onExpired should not have been called yet (2)"));
  tmr.cancel();
  _delay_ms(22);
  helper.doPoll(&tmr, &capture);
  t->verify(capture.callCount == 1, F("onExpired called by cancelled timer"));
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
  t->verify(capture.callCount == 0, F("onExpired should not have been called yet"));
  _delay_ms(22);
  helper.doPoll(&tmr, &capture);
  t->verify(capture.value == 11, F("Expected value = 11"));
  t->verify(capture.callCount == 1, F("onExpired should have been called once"));
  _delay_ms(22);
  helper.doPoll(&tmr, &capture);
  t->verify(capture.callCount == 2, F("onExpired should have been called 2x"));
  _delay_ms(22);
  helper.doPoll(&tmr, &capture);
  t->verify(capture.callCount == 3, F("onExpired should have been called 3x"));
  tmr.cancel();
  _delay_ms(22);
  helper.doPoll(&tmr, &capture);
  t->verify(capture.callCount == 3, F("onExpired called by cancelled timer"));
}

int main() {
  BareMetalHAL::Uart0::begin(9600);
  BareMetalHAL::timingInit();

  TestFunction tests[] = {
    testDigitalPinSourceBasic,
    testButtonBasic,
    testButtonLongPress,
    testToggle,
    testTimer,
    testIntervalTimer
  };

  runTestSuiteShowMem(tests, before, nullptr);

  while (true) {}
  return 0;
}
