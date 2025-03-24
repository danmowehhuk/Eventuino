#include <Eventuino.h>
#include <eventuino/Button.h>

using namespace eventuino;

#define BUTTON1_PIN 5
#define BUTTON1_VALUE 1

#define BUTTON2_PIN 6
#define BUTTON2_VALUE 2

Button button1(BUTTON1_PIN, BUTTON1_VALUE); 
Button button2(BUTTON2_PIN, BUTTON2_VALUE); 
Eventuino evt;

void buttonPressed(uint8_t value) {
  Serial.print("Button pressed with value=");
  Serial.println(value);
}

void buttonLongPressed(uint8_t value) {
  Serial.print("Button long-pressed with value=");
  Serial.println(value);
}

void buttonReleased(uint8_t value) {
  Serial.print("Button released with value=");
  Serial.println(value);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  button1.onPressed=buttonPressed;
  button1.onReleased=buttonReleased;
  button1.onLongPress=buttonLongPressed;

  // Button2 will repeat the onPressed action when long-pressed
  // and has no onReleased action
  button2.onPressed=buttonPressed;
  //button2.onReleased=buttonReleased
  button2.onLongPress=buttonPressed;
  button2.enableRepeat(true);

  evt.addEventSource(&button1);
  evt.addEventSource(&button2);
  evt.begin();
}

void loop() {
  evt.poll();
}
