#ifndef LIGHTS_H
#define LIGHTS_H

#include "SerialReader.h"
#include "pins.h"
#include <Adafruit_TLC5947.h>
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

#define FASTLED_RMT_BUILTIN_DRIVER 1
#define FASTLED_RMT_USE_ASYNC 0

#define FASTLED_RMT_MAX_CHANNELS 1

class Lights {
public:
  Lights();

  ~Lights();

  void begin();

private:
  uint8_t buttons[NUM_BUTTONS] = {
      TOP_1_P1,    TOP_2_P1,    TOP_3_P1,    TOP_4_P1,  BOTTOM_1_P1,
      BOTTOM_2_P1, BOTTOM_3_P1, BOTTOM_4_P1, SELECT_P1, START_P1,
      TOP_1_P2,    TOP_2_P2,    TOP_3_P2,    TOP_4_P2,  BOTTOM_1_P2,
      BOTTOM_2_P2, BOTTOM_3_P2, BOTTOM_4_P2, SELECT_P2, START_P2 //
  };

  uint16_t r[NUM_BUTTONS];
  uint16_t g[NUM_BUTTONS];
  uint16_t b[NUM_BUTTONS];

  CRGB leds[24];
  CRGB onboardLed[1];

  //   Adafruit_TLC5947 buttonLights =
  //       Adafruit_TLC5947(NUM_TLC5947, BUTTON_LIGHTS_CLOCK_PIN,
  //                        BUTTON_LIGHTS_DATA_PIN, BUTTON_LIGHTS_LATCH_PIN);

  static bool buttonLightsStarted;
};

extern Lights lights;

#endif // LIGHTS_H