
#include "pins.h"
#include <Adafruit_TLC5947.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FastLED.h>

#define NUM_TLC5947 3

#define NUM_BUTTONS 20

#define STEP 200

#define SELECT_P1 14
#define START_P1 15
#define TOP_1_P1 0
#define TOP_2_P1 1
#define TOP_3_P1 2
#define TOP_4_P1 3
#define BOTTOM_1_P1 4
#define BOTTOM_2_P1 5
#define BOTTOM_3_P1 6
#define BOTTOM_4_P1 7

#define SELECT_P2 12
#define START_P2 13
#define TOP_1_P2 16
#define TOP_2_P2 17
#define TOP_3_P2 18
#define TOP_4_P2 19
#define BOTTOM_1_P2 20
#define BOTTOM_2_P2 21
#define BOTTOM_3_P2 22
#define BOTTOM_4_P2 23

uint8_t buttons[NUM_BUTTONS] = {
    TOP_1_P1,    TOP_2_P1,    TOP_3_P1,    TOP_4_P1,  BOTTOM_1_P1,
    BOTTOM_2_P1, BOTTOM_3_P1, BOTTOM_4_P1, SELECT_P1, START_P1,
    TOP_1_P2,    TOP_2_P2,    TOP_3_P2,    TOP_4_P2,  BOTTOM_1_P2,
    BOTTOM_2_P2, BOTTOM_3_P2, BOTTOM_4_P2, SELECT_P2, START_P2 //
};

String jsonString = "";
int r[NUM_BUTTONS], g[NUM_BUTTONS], b[NUM_BUTTONS];
int rTarget[NUM_BUTTONS];
int gTarget[NUM_BUTTONS];
int bTarget[NUM_BUTTONS];
Adafruit_TLC5947 tlc =
    Adafruit_TLC5947(NUM_TLC5947, BUTTON_LIGHTS_CLOCK_PIN,
                     BUTTON_LIGHTS_DATA_PIN, BUTTON_LIGHTS_LATCH_PIN);

CRGB leds[24];

bool tlcStarted = false;

void Delay(int x) { vTaskDelay(pdMS_TO_TICKS(x)); }

void writeTask(void *pvParameters) {
  tlc.begin();

  while (1) {

    for (int i = 0; i < NUM_BUTTONS; i++) {
      r[i] = rTarget[i];
      g[i] = gTarget[i];
      b[i] = bTarget[i];
      float rGap = rTarget[i] - r[i];
      int ratio = abs(round(rGap * .1)) + 10;
      if (rGap >= ratio) {
        r[i] += ratio;
        // Serial.println(r[i]);
      } else if (rGap <= -ratio) {
        r[i] -= ratio;
        // Serial.println(r[i]);
      } else {
        r[i] = rTarget[i];
      }

      float gGap = gTarget[i] - g[i];
      ratio = abs(round(gGap * .1)) + 10;
      if (gGap >= ratio) {
        g[i] += ratio;
        // Serial.println(g[i]);
      } else if (gGap <= -ratio) {
        g[i] -= ratio;
        // Serial.println(g[i]);
      } else {
        g[i] = gTarget[i];
      }

      float bGap = bTarget[i] - b[i];
      ratio = abs(round(bGap * .1)) + 10;
      if (bGap >= ratio) {
        b[i] += ratio;
        // Serial.println(b[i]);
      } else if (bGap <= -ratio) {
        b[i] -= ratio;
        // Serial.println(b[i]);
      } else {
        b[i] = bTarget[i];
      }

      tlc.setLED(buttons[i], r[i], g[i], b[i]);
      // tlc.setLED(i >= 8 ? i + 4 : i + 16, r[i], g[i], b[i]);
    }
    tlc.write();
    // Delay(10);
  }
}

void go(TimerHandle_t xTimer) {
  // void go(TimerHandle_t xTimer) {
  if (!tlcStarted) {
    tlc.begin();
    tlcStarted = true;
  }
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.println("JSON parse error");
    return;
  }

  uint8_t numButtons = doc["buttons"].size();

  if (doc["buttons"]) {
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
      // String key = "button" + String(i + 1);
      const char *hex = doc["buttons"][i];

      if (i >= numButtons) {
        int _i = numButtons - NUM_BUTTONS + i;
        hex = doc["buttons"][_i];
      }

      if (hex && strlen(hex) >= 6) {
        char buf[3] = {0};

        buf[0] = hex[0];
        buf[1] = hex[1];
        int r8 = strtol(buf, nullptr, 16);

        buf[0] = hex[2];
        buf[1] = hex[3];
        int g8 = strtol(buf, nullptr, 16);

        buf[0] = hex[4];
        buf[1] = hex[5];
        int b8 = strtol(buf, nullptr, 16);

        rTarget[i] = (r8 * 4095 + 127) / 255;
        gTarget[i] = (g8 * 4095 + 127) / 255;
        bTarget[i] = (b8 * 4095 + 127) / 255;

        // uint8_t r = (hex >> 16) & 0xFF;
        // uint8_t g = (hex >> 8) & 0xFF;
        // uint8_t b = hex & 0xFF;

      } else {
        rTarget[i] = gTarget[i] = bTarget[i] = 0;
      }

      tlc.setLED(buttons[i], rTarget[i], gTarget[i], bTarget[i]);
    }
    tlc.write();
  }
  if (doc["sticks"]) {
  }
}

// void lightsTask(void *pvParameters) {
//   // StaticJsonDocument<512> doc;
//   Serial.setTimeout(10); // Don't block for long
//   go(NULL);
//   while (1) {
//     if (Serial.available()) {
//       jsonString = Serial.readStringUntil('\n');
//       Serial.println(jsonString);

//       for (int i = 0; i < NUM_BUTTONS; i++) {
//         rTarget[i] = gTarget[i] = bTarget[i] = 0;
//       }

//       TimerHandle_t timeout =
//           xTimerCreate("Timeout", pdMS_TO_TICKS(400), pdFALSE, NULL, go);
//       xTimerStart(timeout, 0);
//     }

//     vTaskDelay(1); // Yield to FreeRTOS scheduler
//   }
// }
