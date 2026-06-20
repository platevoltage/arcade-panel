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
  sensor.setResolution(MLX90393_X, MLX90393_RES_18);
  sensor.setResolution(MLX90393_Y, MLX90393_RES_18);
  sensor.setResolution(MLX90393_Z, MLX90393_RES_16);

  // Set oversampling
  sensor.setOversampling(MLX90393_OSR_3);

  // Set digital filtering
  sensor.setFilter(MLX90393_FILTER_0);
};

// void pressButton(hid_gamepad_button_bm_t buttonIndex) {
//   player1.gp.buttons |= (1U << buttonIndex);
// }

void pressButton(hid_gamepad_button_bm_t buttonIndex) {
  player1.gp.buttons |= buttonIndex;
}

void releaseButton(hid_gamepad_button_bm_t buttonIndex) {
  player1.gp.buttons &= ~buttonIndex;
}

// void releaseButton(hid_gamepad_button_bm_t buttonIndex) {
//   // player1.gp.buttons |= (1U << buttonIndex);
//   // usb_hid.sendReport(0, &gp, sizeof(gp));
//   // delay(holdMs);

//   player1.gp.buttons &= ~(1U << buttonIndex);
//   // usb_hid.sendReport(0, &gp, sizeof(gp));
// }

void Analog::task() {

  sensors_event_t event;
  sensor.getEvent(&event);
  /* Display the results (magnetic field is measured in uTesla) */

  float x = event.magnetic.x / 400.0f;
  float y = -event.magnetic.y / 400.0f;

  if (x > 127)
    x = 127;
  else if (x < -128)
    x = -128;
  if (y > 127)
    y = 127;
  else if (y < -128)
    y = -128;

  player1.gp.x = x;
  player1.gp.y = y;

  player2.gp.x = x;
  player2.gp.y = y;

  if (x > 100) {
    player1.gp.hat = GAMEPAD_HAT_RIGHT;
    player2.gp.hat = GAMEPAD_HAT_RIGHT;
  } else {
    player1.gp.hat = GAMEPAD_HAT_CENTERED;
    player2.gp.hat = GAMEPAD_HAT_CENTERED;
  }

  if (event.magnetic.z < 0) {
    pressButton(GAMEPAD_BUTTON_TL2);
  } else {
    releaseButton(GAMEPAD_BUTTON_TL2);
  }

  // if (abs(event.magnetic.x) > 5000 || abs(event.magnetic.y) > 5000) {
  // Serial.print("X: ");
  // Serial.print(x);
  // Serial.print(" \tY: ");
  // Serial.print(y);
  // // }
  // Serial.print(" \tZ: ");
  // Serial.println(event.magnetic.z);
};