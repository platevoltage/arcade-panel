#include "Analog.h"

Analog::Analog() {};
Analog::~Analog() {};

Adafruit_MLX90393 Analog::sensor = Adafruit_MLX90393();

Analog analog;

void Analog::begin() {
  Wire.setSCL(17);
  Wire.setSDA(16);
  //   if (!sensor.begin_I2C()) { // hardware SPI mode
  if (!sensor.begin_I2C(0x0C)) { // hardware SPI mode
    while (1) {
      Serial.println("No sensor found ... check your wiring?");
      delay(10);
    }
  }

  Serial.println("Found a MLX90393 sensor");

  sensor.setGain(MLX90393_GAIN_5X);

  // Set resolution, per axis. Aim for sensitivity of ~0.3 for all axes.
  sensor.setResolution(MLX90393_X, MLX90393_RES_19);
  sensor.setResolution(MLX90393_Y, MLX90393_RES_19);
  sensor.setResolution(MLX90393_Z, MLX90393_RES_19);

  // Set oversampling
  sensor.setOversampling(MLX90393_OSR_3);

  // Set digital filtering
  sensor.setFilter(MLX90393_FILTER_0);
};
void Analog::task() {

  sensors_event_t event;
  sensor.getEvent(&event);
  /* Display the results (magnetic field is measured in uTesla) */

  int8_t x = event.magnetic.x / 512.0f;
  int8_t y = event.magnetic.y / 512.0f;
  player1.gp.x = x;
  player1.gp.y = y;

  if (x > 100) {
    player1.gp.hat = GAMEPAD_HAT_RIGHT;
  } else {
    player1.gp.hat = GAMEPAD_HAT_CENTERED;
  }

  if (abs(event.magnetic.x) > 5000 || abs(event.magnetic.y) > 5000) {
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" \tY: ");
    Serial.println(y);
  }
  // Serial.print(" \tZ: ");
  // Serial.print(event.magnetic.z);
};