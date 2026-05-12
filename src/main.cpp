
#include <Arduino.h>

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "keyboard.h"

char jsonBuffer[512];
void lightsTask() {

  USBSerial.setTimeout(10);

  while (1) {

    int len =
        USBSerial.readBytesUntil('\n', jsonBuffer, sizeof(jsonBuffer) - 1);

    if (len > 0) {

      jsonBuffer[len] = '\0';
      // Serial.println(len);
      if (jsonBuffer[0] == '{') {
        // parse later
        for (int i = 0; i < NUM_BUTTONS; i++) {
          r[i] = g[i] = b[i] = 0;
        }
        jsonString = String(jsonBuffer);
        go();
      } else {
        typeKey(jsonBuffer[0], KEY_LEFT_SHIFT);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  USB.productName("ARCADE THING");
  USBSerial.begin(115200);
  USBSerial.setTxTimeoutMs(0);
  Serial.begin(115200);
  buttonLights.begin();
  FastLED.addLeds<NEOPIXEL, RING_DATA_PIN>(leds, 24);
  FastLED.addLeds<NEOPIXEL, ONBOARD_RGB_DATA_PIN>(onboardLed, 1);
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

  delay(10000);
  xTaskCreatePinnedToCore(keyboardTask,   // Function to run
                          "keyboardTask", // Name
                          4096,           // Stack size
                          NULL,           // Parameters
                          1,              // Priority
                          NULL,           // Task handle
                          0);
}

void loop() { lightsTask(); }
