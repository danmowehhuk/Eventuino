#include "Toggle.h"

using namespace eventuino;

void Toggle::onChange(uint8_t value, void* state = nullptr) {
  if (isActive() && onActivate != 0) {
    onActivate(value, state);
  } else if (!isActive() && onDeactivate != 0) {
    onDeactivate(value, state);
  }
  if (onFlip != 0) {
    onFlip(value, state);
  }
}

bool Toggle::isActivated() {
  return isActive();
}

void Toggle::clearCallbacks() {
  onFlip = 0;
  onActivate = 0;
  onDeactivate = 0;
}

Toggle::Toggle(Toggle&& other) noexcept: DigitalPinSource(move(other)) {
  onFlip = other.onFlip;
  onActivate = other.onActivate;
  onDeactivate = other.onDeactivate;
  other.onFlip = 0;
  other.onActivate = 0;
  other.onDeactivate = 0;
}

Toggle& Toggle::operator=(Toggle&& other) noexcept {
  if (this != &other) {
    DigitalPinSource::operator=(move(other));
    onFlip = other.onFlip;
    onActivate = other.onActivate;
    onDeactivate = other.onDeactivate;
    other.onFlip = 0;
    other.onActivate = 0;
    other.onDeactivate = 0;
  }
  return *this;
}
