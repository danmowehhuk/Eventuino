#include <Eventuino.h>
#include <eventuino/Button.h>

using namespace eventuino;

#define BUTTON_PIN 5
#define BUTTON_VALUE 1

Button button(BUTTON_PIN, BUTTON_VALUE); 
Eventuino evt;
EventSource* eventSources[] = { &button };

void buttonPressed(uint8_t value) {
  Serial.print("Button pressed with value=");
  Serial.println(value);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Callbacks can be changed at any time and take effect with 
  // the next poll
  button.onPressed=buttonPressed;

  evt.setEventSources(eventSources, 1);
  evt.begin();
}

void loop() {
  evt.poll();
}
