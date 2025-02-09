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
