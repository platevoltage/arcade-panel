#include "Gamepad.h"
#include "Keyboard.h"
#include "Storage.h"
#include <Arduino.h>

bool core1_separate_stack = true;

void setup() {
  Serial.begin(115200);
  gamepad.begin();
  keyboard.begin();
  storage.begin();
  delay(5000);
  Serial.print("Free stack: ");
  Serial.println(rp2040.getFreeStack());
}

void loop() {
  gamepad.task();
  keyboard.task();
  storage.task();
}

// NO USB STUFF ON CORE 1
void setup1() {
  delay(5000);
  //
}
void loop1() {
  // Serial.println(Serial.available());
  Serial.println("loop 1");
  delay(1500);
}