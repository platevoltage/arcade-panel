#include "Gamepad.h"
#include "Keyboard.h"
#include "Lights.h"
#include "SerialReader.h"
#include "Storage.h"

bool core1_separate_stack = true;

void setup() {
  Serial.begin(115200);
  Serial1.setTX(12);
  Serial1.setRX(13);
  Serial1.begin(115200);
  USBDevice.setManufacturerDescriptor("Laika");
  USBDevice.setProductDescriptor("Arcade Panel");

  player1.begin();
  player2.begin();
  keyboard.begin();
  storage.begin();
  delay(5000);
  Serial.print("Free stack: ");
  Serial.println(rp2040.getFreeStack());
}

void loop() {
  player1.task();
  player2.task();
  storage.task();
  serialReader.task();
  Serial.println("hello world!");
  // Serial1.println("hello world11!");
  delay(1000);
}

// NO USB STUFF ON CORE 1
void setup1() {
  delay(5000);
  // lights.begin();
  //
}
void loop1() {
  // Serial.println(Serial.available());2
  // Serial.println("loop 1");
  Serial1.println("hello world1!");
  delay(1500);
}