#include "Lights.h"

Lights::Lights() {};

Lights::~Lights() {};

Lights lights;

bool Lights::buttonLightsStarted = false;

void Lights::begin() {
  while (1) {
    if (!buttonLightsStarted) {
      // TURN THIS BACK ON ONCE YOU FIGURE OUT PINS
      //   buttonLights.begin();
      buttonLightsStarted = true;
    }
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, serialReader.jsonString);

    if (error) {
      // Serial.println("JSON parse error");

    } else {

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

          //   buttonLights.setLED(buttons[i], r, g, b);
        }
        // buttonLights.write();
      }
      if (doc["sticks"]) {
        for (int i = 0; i < 24; i++) {
          uint8_t r = doc["sticks"][i]["r"];
          uint8_t g = doc["sticks"][i]["g"];
          uint8_t b = doc["sticks"][i]["b"];
          leds[i] = CRGB(r, g, b);
        }
        FastLED.show();
      }
    }
    delay(25);
  }
}
