// #include "gamepad.h"
#include "keyboard.h"
#include "lights.h"
#include <Arduino.h>

char jsonBuffer[512];
void lightsTask(void *pvParameters) {

  Serial.setTimeout(10);

  while (1) {

    int len = Serial.readBytesUntil('\n', jsonBuffer, sizeof(jsonBuffer) - 1);

    if (len > 0) {

      jsonBuffer[len] = '\0';
      // Serial.println(len);
      if (jsonBuffer[0] == '{') {
        // parse later
        for (int i = 0; i < NUM_BUTTONS; i++) {
          rTarget[i] = gTarget[i] = bTarget[i] = 0;
        }
        jsonString = String(jsonBuffer);
        go(NULL);
      } else {
        typeKey(jsonBuffer[0], KEY_LEFT_SHIFT);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  Serial.begin(115200);
  // Serial.setTxTimeoutMs(0);
  tlc.begin();
  FastLED.addLeds<NEOPIXEL, RING_DATA_PIN>(leds, 24);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    tlc.setLED(i, 0, 0, 0);
  }
  tlc.write();
  // pinMode(L2_PIN, INPUT_PULLUP);
  // pinMode(R2_PIN, INPUT_PULLUP);

  Keyboard.begin();
  USB.begin();
  // Gamepad.begin();
  // Keyboard.begin();
  // USB.begin();

  xTaskCreatePinnedToCore(keyboardTask,   // Function to run
                          "keyboardTask", // Name
                          4096,           // Stack size
                          NULL,           // Parameters
                          1,              // Priority
                          NULL,           // Task handle
                          0);
}

void loop() {
  lightsTask(NULL);
  // gamepadTask();
  // keyboardTask();

  // Serial.printf("Free heap: %d\n", ESP.getFrew2eHeap());
  // Serial.printf("Minimum free heap: %d\n", ESP.getMinFreeHeap());
  // Serial.printf("Maximum allocatable block: %d\n",
  //               heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT));
}
