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
  void begin();
  void task();
  Adafruit_USBD_HID player;
  // static Adafruit_USBD_HID player1;
  // static Adafruit_USBD_HID player2;

private:
  static const uint8_t desc_hid_report[];
};

// extern Gamepad gamepad;
extern Gamepad player1;
extern Gamepad player2;

#endif // GAMEPAD_H