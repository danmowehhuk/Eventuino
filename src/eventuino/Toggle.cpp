#include "Toggle.h"

using namespace eventuino;

void Toggle::onChange(uint8_t value) {
  if (isActive() && onActivate != 0) {
    onActivate(value);
  } else if (!isActive() && onDeactivate != 0) {
    onDeactivate(value);
  }
  if (onFlip != 0) {
    onFlip(value);
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
