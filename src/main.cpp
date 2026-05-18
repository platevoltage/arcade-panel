// "filesystem": "littlefs",
// "partitions": "partitions.csv"
#include <Arduino.h>

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "keyboard.hpp"

char jsonBuffer[4096];

void pollUSB() {

  static int len = 0;

  while (USBSerial.available()) {

    char c = USBSerial.read();
    // UART.print(c);
    if (c == '\n') {

      jsonBuffer[len] = '\0';

      if (len > 0) {

        if (jsonBuffer[0] == '{') {

          for (int i = 0; i < NUM_BUTTONS; i++) {
            r[i] = g[i] = b[i] = 0;
          }

          jsonString = String(jsonBuffer);
          // UART.println(jsonString);

        } else {
          typeKey(jsonBuffer[0], KEY_LEFT_SHIFT);
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

void setup() {
  storageSetup();
  USB.productName("ARCADE THING");
  USBSerial.begin(115200);
  USBSerial.setTxTimeoutMs(0);
  USBSerial.setRxBufferSize(0xFFFF);
  UART.begin(115200);
  buttonLights.begin();
  FastLED.addLeds<NEOPIXEL, RING_DATA_PIN>(leds, 24);
  FastLED.addLeds<NEOPIXEL, ONBOARD_RGB_DATA_PIN>(onboardLed, 1);

  for (int i = 0; i < 24; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();

  USBSerial.print("IM ALIVE");
  for (int i = 0; i < NUM_BUTTONS; i++) {
    USBSerial.print(buttonPins[i]);
    USBSerial.print(" / ");
    pinMode(buttonPins[i], INPUT_PULLUP);
    buttonLights.setLED(i, 0, 0, 0);
  }
  buttonLights.write();

  // delay(2000);
  USB.begin();
  Keyboard.begin();
  delay(4000);
  // lightsTask(NULL);
  xTaskCreatePinnedToCore(keyboardTask,   // Function to run
                          "keyboardTask", // Name
                          4096,           // Stack size
                          NULL,           // Parameters
                          1,              // Priority
                          NULL,           // Task handle
                          0);
  xTaskCreatePinnedToCore(go,   // Function to run
                          "go", // Name
                          4096, // Stack size
                          NULL, // Parameters
                          1,    // Priority
                          NULL, // Task handle
                          1);
  // xTaskCreatePinnedToCore(lightsTask, // Function to run
  //                         "lights",   // Name
  //                         0x4000,     // Stack size
  //                         NULL,       // Parameters
  //                         1,          // Priority
  //                         NULL,       // Task handle
  //                         1);
}

void loop() { pollUSB(); }
