#include "lights.hpp"
#include "pins.hpp"
#include "storage.hpp"
#include <Arduino.h>

USBCDC USBSerial;

USBHIDKeyboard Keyboard;
bool pauseProbing = false;

#define _KEY_0_P1 'z'
#define _KEY_1_P1 'x'
#define _KEY_2_P1 'q'
#define _KEY_3_P1 'w'
#define _KEY_4_P1 'a'
#define _KEY_5_P1 's'
#define _KEY_6_P1 '1'
#define _KEY_7_P1 '2'
#define _KEY_8_P1 ' '
#define _KEY_9_P1 KEY_RETURN

#define _KEY_0_P2 't'
#define _KEY_1_P2 'y'
#define _KEY_2_P2 'u'
#define _KEY_3_P2 'i'
#define _KEY_4_P2 'o'
#define _KEY_5_P2 'f'
#define _KEY_6_P2 '3'
#define _KEY_7_P2 '4'
#define _KEY_8_P2 'g'
#define _KEY_9_P2 'j'

#define NUM_BUTTONS 20

const uint8_t buttonPins[NUM_BUTTONS] = {
    BUTTON_PIN_0_P1, BUTTON_PIN_1_P1, BUTTON_PIN_2_P1, BUTTON_PIN_3_P1,
    BUTTON_PIN_4_P1, BUTTON_PIN_5_P1, BUTTON_PIN_6_P1, BUTTON_PIN_7_P1,
    BUTTON_PIN_8_P1, BUTTON_PIN_9_P1, BUTTON_PIN_0_P2, BUTTON_PIN_1_P2,
    BUTTON_PIN_2_P2, BUTTON_PIN_3_P2, BUTTON_PIN_4_P2, BUTTON_PIN_5_P2,
    BUTTON_PIN_6_P2, BUTTON_PIN_7_P2, BUTTON_PIN_8_P2, BUTTON_PIN_9_P2};

const uint8_t keys[NUM_BUTTONS] = {
    _KEY_0_P1, _KEY_1_P1, _KEY_2_P1, _KEY_3_P1, _KEY_4_P1, _KEY_5_P1, _KEY_6_P1,
    _KEY_7_P1, _KEY_8_P1, _KEY_9_P1, _KEY_0_P2, _KEY_1_P2, _KEY_2_P2, _KEY_3_P2,
    _KEY_4_P2, _KEY_5_P2, _KEY_6_P2, _KEY_7_P2, _KEY_8_P2, _KEY_9_P2,
};

bool buttonState[NUM_BUTTONS] = {false};

bool probeButton(uint8_t buttonPin, uint8_t buttonNumber) {
  if (!digitalRead(buttonPin)) {
    // Serial.print(buttonNumber);
    if (!buttonState[buttonNumber]) {
      Keyboard.press(keys[buttonNumber]);
      buttonState[buttonNumber] = true;
    }
    return true;
  } else {
    if (buttonState[buttonNumber]) {
      Keyboard.release(keys[buttonNumber]);
      buttonState[buttonNumber] = false;
    }
    return false;
  }
}

void keyboardTask(void *pvParameters) {
  while (1) {
    bool buttonPushed = false;
    if (!pauseProbing) {
      for (int i = 0; i < NUM_BUTTONS; i++) {
        if (probeButton(buttonPins[i], i)) {
          buttonPushed = true;
        }
      }
    }
    // if (buttonPushed) {
    //   onboardLed[0] = CRGB::Blue;
    //   FastLED.show();
    // } else {
    //   onboardLed[0] = CRGB::Black;
    //   FastLED.show();
    // }
    vTaskDelay(1);
  }
}

void typeKey(char key, char mod = 0) {
  pauseProbing = true;
  vTaskDelay(pdMS_TO_TICKS(10));
  Keyboard.releaseAll();
  if (mod) {
    Keyboard.press(mod);
  }

  Keyboard.press(key);

  Serial.print("Print key - ");
  Serial.print(key);
  Serial.print(" - ");
  Serial.println(key, HEX);
  vTaskDelay(pdMS_TO_TICKS(100));
  Keyboard.release(key);
  Serial.print("Release key - ");
  Serial.println(key);
  if (mod) {
    Keyboard.release(mod);
  }
  pauseProbing = false;
}