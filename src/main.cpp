#include "Gamepad.h"
#include "Keyboard.h"
#include "Storage.h"
#include <Arduino.h>
// bool core1_disable_systick = true;
// bool core1_separate_stack = true;

void setup() {
  Serial.begin(115200);
  gamepad.begin();
  keyboard.begin();
  storage.begin();
}

void loop() {
  gamepad.task();
  keyboard.task();
  // Serial.println("loop 0");
  // delay(500);
}
void setup1() { delay(5000); }
void loop1() {
  // Serial.println(Serial.available());
  // Serial.println("loop 1");
  delay(1500);
}