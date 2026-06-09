#include "SerialReader.h"

SerialReader::SerialReader() {}

SerialReader::~SerialReader() {}

SerialReader serialReader;

char SerialReader::jsonBuffer[4096];
String SerialReader::jsonString = "";

void SerialReader::task() {

  static int len = 0;

  while (Serial.available()) {

    char c = Serial.read();
    // Serial.println(c);
    if (c == '\n') {

      jsonBuffer[len] = '\0';

      if (len > 0) {

        if (jsonBuffer[0] == '{') {

          //   for (int i = 0; i < NUM_BUTTONS; i++) {
          //     r[i] = g[i] = b[i] = 0;
          //   }

          jsonString = String(jsonBuffer);
          Serial.println("RESPONSE");
          // Serial.print("---------");
          // Serial.println(jsonString);

          //   if (TinyUSBDevice.mounted()) {
          //     TinyUSBDevice.detach();
          //     delay(10);
          //     const char *msg =
          //         "This is where frequently changing configs will go";
          //     storage.write_ram_file("TEST    JSN", (uint8_t *)jsonBuffer,
          //                            strlen(jsonBuffer));
          //     TinyUSBDevice.attach();
          //   }

        } else {
          // typeKey(jsonBuffer[0], KEY_LEFT_SHIFT);
        }
      }

      len = 0; // reset line buffer
    } else {
      if (len < (int)sizeof(jsonBuffer) - 1) {
        jsonBuffer[len++] = c;
      } else {
        // overflow protection: reset line
        len = 0;
      }
    }
  }
}