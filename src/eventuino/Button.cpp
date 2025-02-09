#include "Button.h"

using namespace eventuino;

void Button::onChange(uint8_t value, void* state = nullptr) {
  if (isActive() && onPressed != 0) {
    onPressed(value, state);
  } else if (!isActive() && onReleased != 0) {
    onReleased(value, state);
  }
}

void Button::onLongHold(uint8_t value, void* state = nullptr) {
  if (onLongPress != 0) {
    onLongPress(value, state);
  }
}

bool Button::isPressed() {
  return isActive();
}

bool Button::isLongPressed() {
  return isLongHold();
}

void Button::clearCallbacks() {
  onPressed = 0;
  onReleased = 0;
  onLongPress = 0;
}
