#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <Arduino.h>

class SerialReader {
public:
  SerialReader();

  ~SerialReader();

  void task();
  static String jsonString;

private:
  static char jsonBuffer[4096];
};

extern SerialReader serialReader;

#endif // SERIALREADER_H