#include "Button.h"

using namespace eventuino;

void Button::onChange(uint8_t value) {
  if (isActive() && onPressed != 0) {
    onPressed(value);
  } else if (!isActive() && onReleased != 0) {
    onReleased(value);
  }
}

void Button::onLongHold(uint8_t value) {
  if (onLongPress != 0) {
    onLongPress(value);
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
