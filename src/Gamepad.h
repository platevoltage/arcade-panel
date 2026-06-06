#ifndef GAMEPAD_H
#define GAMEPAD_H

#include "Adafruit_TinyUSB.h"

class Gamepad {
public:
  // Constructor
  Gamepad();

  // Destructor
  ~Gamepad();

  // Member functions
  static void begin();
  static void task();
  static Adafruit_USBD_HID player1;
  static Adafruit_USBD_HID player2;

private:
  static const uint8_t desc_hid_report[];
};

// extern Adafruit_USBD_HID player1;
// extern Adafruit_USBD_HID player2;

#endif // GAMEPAD_H