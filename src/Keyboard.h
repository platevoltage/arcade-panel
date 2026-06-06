#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "Adafruit_TinyUSB.h"

class Keyboard {
public:
  // Constructor
  Keyboard();

  // Destructor
  ~Keyboard();

  // Member functions
  static void begin();
  static void task();
  static Adafruit_USBD_HID keyboard;

private:
  static const uint8_t desc_hid_report[];
  static void process_hid();
  static uint8_t pins[];
  static uint8_t pincount;
  static uint8_t hidcode[];
  static bool activeState;
};

// extern Adafruit_USBD_HID player1;
// extern Adafruit_USBD_HID player2;

#endif // KEYBOARD_H