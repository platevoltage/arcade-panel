#include "Gamepad.h"
#include "Keyboard.h"
#include "Storage.h"
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  gamepad.begin();
  keyboard.begin();
  storage.begin();
}

void loop() {
  gamepad.task();
  keyboard.task();
}