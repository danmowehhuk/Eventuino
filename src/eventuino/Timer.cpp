#include <Arduino.h>
#include "Timer.h"

void Timer::poll() {
  if (!_isActive) return;
  if (millis() - _startTime >= _duration && onExpire != 0) {
    onExpire(_value);
    cancel();
  }
}

void Timer::start(uint32_t duration) {
  _duration = duration;
  _startTime = millis();
  _isActive = true;
}

void Timer::cancel() {
  _isActive = false;
  _startTime = 0;
  _duration = 0;
}
