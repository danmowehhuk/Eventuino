#include <Eventuino.h>
#include <eventuino/Toggle.h>

using namespace eventuino;

#define TOGGLE_PIN 5
#define TOGGLE_VALUE 1

Toggle toggle(TOGGLE_PIN, TOGGLE_VALUE); 
Eventuino evt;

void toggleFlipped(uint8_t value) {
  Serial.print("Toggle flipped with value=");
  Serial.println(value);
}
void toggleActivated(uint8_t value) {
  Serial.print("Toggle ");
  Serial.print(value);
  Serial.println(" has been activated");
}
void toggleDeactivated(uint8_t value) {
  Serial.print("Toggle ");
  Serial.print(value);
  Serial.println(" has been deactivated");
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Callbacks can be changed at any time and take effect with 
  // the next poll
  toggle.onFlip=toggleFlipped;
  toggle.onActivate=toggleActivated;
  toggle.onDeactivate=toggleDeactivated;

  evt.addEventSource(&toggle);
  evt.begin();
}

void loop() {
  evt.poll();
}
