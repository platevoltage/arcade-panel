#include "Gamepad.h"
#include <Arduino.h>

Gamepad gamepad;

void setup() {
  //
  Serial.begin(115200);
  gamepad.begin();
}

void loop() {
  //
  gamepad.task();
}