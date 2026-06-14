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

  sensor.setGain(MLX90393_GAIN_1X);
  // You can check the gain too
  Serial.print("Gain set to: ");
  switch (sensor.getGain()) {
  case MLX90393_GAIN_1X:
    Serial.println("1 x");
    break;
  case MLX90393_GAIN_1_33X:
    Serial.println("1.33 x");
    break;
  case MLX90393_GAIN_1_67X:
    Serial.println("1.67 x");
    break;
  case MLX90393_GAIN_2X:
    Serial.println("2 x");
    break;
  case MLX90393_GAIN_2_5X:
    Serial.println("2.5 x");
    break;
  case MLX90393_GAIN_3X:
    Serial.println("3 x");
    break;
  case MLX90393_GAIN_4X:
    Serial.println("4 x");
    break;
  case MLX90393_GAIN_5X:
    Serial.println("5 x");
    break;
  }

  // Set resolution, per axis. Aim for sensitivity of ~0.3 for all axes.
  sensor.setResolution(MLX90393_X, MLX90393_RES_17);
  sensor.setResolution(MLX90393_Y, MLX90393_RES_17);
  sensor.setResolution(MLX90393_Z, MLX90393_RES_16);

  // Set oversampling
  sensor.setOversampling(MLX90393_OSR_3);

  // Set digital filtering
  sensor.setFilter(MLX90393_FILTER_5);
};
void Analog::task() {
  float x, y, z;

  // get X Y and Z data at once
  if (sensor.readData(&x, &y, &z)) {
    Serial.print("X: ");
    Serial.print(x, 4);
    Serial.println(" uT");
    Serial.print("Y: ");
    Serial.print(y, 4);
    Serial.println(" uT");
    Serial.print("Z: ");
    Serial.print(z, 4);
    Serial.println(" uT");
  } else {
    Serial.println("Unable to read XYZ data from the sensor.");
  }
};