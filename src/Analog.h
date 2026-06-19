#ifndef ANALOG_H
#define ANALOG_H

#include "Gamepad.h"
#include <Adafruit_MLX90393.h>

class Analog {
  // class implementation goes here
public:
  Analog();
  ~Analog();
  static Adafruit_MLX90393 sensor;

  void begin();
  void task();
};

extern Analog analog;

#endif // ANALOG_H