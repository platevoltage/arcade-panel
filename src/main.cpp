#include "Gamepad.h"
// #include "HC4067.h"
#include "Keyboard.h"
#include "Lights.h"
#include "SerialReader.h"
#include "Storage.h"
#include <Adafruit_MLX90393.h>

bool core1_separate_stack = true;

// HC4067 mp(2, 3, 4, 5, 6); //  enable pin(8)

// const int inputPin = 7;
// uint16_t values;

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

  // Serial.println();
  // Serial.println(__FILE__);
  // Serial.print("HC4067_LIB_VERSION: ");
  // Serial.println(HC4067_LIB_VERSION);
  // Serial.println();

  // pinMode(inputPin, INPUT_PULLDOWN);
}

void loop() {
  player1.task();
  player2.task();
  storage.task();
  serialReader.task();
  Serial.println("hello world!");
  // Serial1.println("hello world11!");
  delay(1000);

  // for (int ch = 0; ch < 16; ch++) {
  //   mp.setChannel(ch);
  //   delayMicroseconds(10); // give the mux time to settle
  //   int val = digitalRead(inputPin);
  //   Serial.print("CH");
  //   Serial.print(ch);
  //   Serial.print(": ");
  //   Serial.println(val);
  //   // mp.enable();
  //   // Serial.println(mp.isEnabled()); // should print 1
  // }
  // Serial.println("---");
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