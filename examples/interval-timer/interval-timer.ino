#include <Eventuino.h>
#include <eventuino/Timer.h>

using namespace eventuino;

#define TIMER_VALUE 1

IntervalTimer14Bit timer(TIMER_VALUE); 
Eventuino evt;
EventSource* eventSources[] = { &timer };

void timerExpired(uint8_t value) {
  Serial.print("Timer expired at: ");
  Serial.println(millis());
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Callbacks can be changed at any time and take effect with 
  // the next poll
  timer.onExpire=timerExpired;

  timer.start(500);

  evt.setEventSources(eventSources, 1);
  evt.begin();
}

void loop() {
  evt.poll();
}
