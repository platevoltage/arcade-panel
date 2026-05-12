
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

int r[NUM_BUTTONS];
int g[NUM_BUTTONS];
int b[NUM_BUTTONS];
Adafruit_TLC5947 buttonLights =
    Adafruit_TLC5947(NUM_TLC5947, BUTTON_LIGHTS_CLOCK_PIN,
                     BUTTON_LIGHTS_DATA_PIN, BUTTON_LIGHTS_LATCH_PIN);

CRGB leds[24];
CRGB onboardLed[1];

bool buttonLightsStarted = false;

void Delay(int x) { vTaskDelay(pdMS_TO_TICKS(x)); }

void go() {

  if (!buttonLightsStarted) {
    buttonLights.begin();
    buttonLightsStarted = true;
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

      // const char *hex = doc["buttons"][i];

      // if (i >= numButtons) {
      //   int _i = numButtons - NUM_BUTTONS + i;
      //   hex = doc["buttons"][_i];
      // }

      // if (hex && strlen(hex) >= 6) {
      //   char buf[3] = {0};

      //   buf[0] = hex[0];
      //   buf[1] = hex[1];
      //   int r8 = strtol(buf, nullptr, 16);

      //   buf[0] = hex[2];
      //   buf[1] = hex[3];
      //   int g8 = strtol(buf, nullptr, 16);

      //   buf[0] = hex[4];
      //   buf[1] = hex[5];
      //   int b8 = strtol(buf, nullptr, 16);

      //   r[i] = (r8 * 4095 + 127) / 255;
      //   g[i] = (g8 * 4095 + 127) / 255;
      //   b[i] = (b8 * 4095 + 127) / 255;

      // } else {
      //   r[i] = g[i] = b[i] = 0;
      // }

      int color = doc["buttons"][i];

      if (i >= numButtons) {
        int _i = numButtons - NUM_BUTTONS + i;
        color = doc["buttons"][_i];
      }

      r[i] = (color >> 16) & 0xFF;
      g[i] = (color >> 8) & 0xFF;
      b[i] = color & 0xFF;

      // Serial.print(i);
      // Serial.print(") ");
      // Serial.print(color);
      // Serial.print(" / ");
      // Serial.print(r[i]);
      // Serial.print(" ");
      // Serial.print(g[i]);
      // Serial.print(" ");
      // Serial.print(b[i]);
      // Serial.print(" / ");
      // Serial.print(r[i], HEX);
      // Serial.print(" ");
      // Serial.print(g[i], HEX);
      // Serial.print(" ");
      // Serial.print(b[i], HEX);
      // Serial.println(" ");

      r[i] = map(r[i], 0, 255, 0, 4095);
      g[i] = map(g[i], 0, 255, 0, 4095);
      b[i] = map(b[i], 0, 255, 0, 4095);
      buttonLights.setLED(buttons[i], r[i], g[i], b[i]);
    }
    buttonLights.write();
  }
  if (doc["sticks"]) {
  }
}
