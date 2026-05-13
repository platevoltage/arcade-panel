
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

uint16_t r[NUM_BUTTONS];
uint16_t g[NUM_BUTTONS];
uint16_t b[NUM_BUTTONS];
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

      // uint64_t color = doc["buttons"][i];
      uint16_t r = doc["buttons"][i]["r"];
      uint16_t g = doc["buttons"][i]["g"];
      uint16_t b = doc["buttons"][i]["b"];

      if (i >= numButtons) {
        int _i = numButtons - NUM_BUTTONS + i;
        // color = doc["buttons"][_i];
        r = doc["buttons"][_i]["r"];
        g = doc["buttons"][_i]["g"];
        b = doc["buttons"][_i]["b"];
      }

      // r[i] = (color >> 32) & 0xFFFF;
      // g[i] = (color >> 16) & 0xFFFF;
      // b[i] = color & 0xFFFF;

      Serial.print(i);
      Serial.print(") ");
      // Serial.print(color);
      // Serial.print(" / ");
      Serial.print(r);
      Serial.print(" ");
      Serial.print(g);
      Serial.print(" ");
      Serial.print(b);
      Serial.print(" / ");
      Serial.print(r, HEX);
      Serial.print(" ");
      Serial.print(g, HEX);
      Serial.print(" ");
      Serial.print(b, HEX);
      Serial.println(" ");

      // r[i] = map(r[i], 0, 255, 0, 4095);
      // g[i] = map(g[i], 0, 255, 0, 4095);
      // b[i] = map(b[i], 0, 255, 0, 4095);
      buttonLights.setLED(buttons[i], r, g, b);
    }
    buttonLights.write();
  }
  if (doc["sticks"]) {
  }
}
