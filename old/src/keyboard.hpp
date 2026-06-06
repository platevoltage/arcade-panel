#include "lights.hpp"
#include "pins.hpp"
// #include "storage.hpp"
#include <Arduino.h>
#include <Preferences.h>

TaskHandle_t keyboardTaskHandle = NULL;
USBCDC USBSerial;
USBHIDKeyboard Keyboard;
Preferences preferences;
bool pauseProbing = false;

char _KEY_0_P1 = 'z';
char _KEY_1_P1 = 'x';
char _KEY_2_P1 = 'q';
char _KEY_3_P1 = 'w';
char _KEY_4_P1 = 'a';
char _KEY_5_P1 = 's';
char _KEY_6_P1 = '1';
char _KEY_7_P1 = '2';
char _KEY_8_P1 = KEY_SPACE;
char _KEY_9_P1 = KEY_RETURN;

char _KEY_0_P2 = 't';
char _KEY_1_P2 = 'y';
char _KEY_2_P2 = 'u';
char _KEY_3_P2 = 'i';
char _KEY_4_P2 = 'o';
char _KEY_5_P2 = 'f';
char _KEY_6_P2 = '3';
char _KEY_7_P2 = '4';
char _KEY_8_P2 = 'g';
char _KEY_9_P2 = 'j';

#define NUM_BUTTONS 20

struct KeyName {
  uint8_t code;
  const char *name;
};

const KeyName keyMap[] = {{KEY_LEFT_CTRL, "KEY_LEFT_CTRL"},
                          {KEY_LEFT_SHIFT, "KEY_LEFT_SHIFT"},
                          {KEY_LEFT_ALT, "KEY_LEFT_ALT"},
                          {KEY_LEFT_GUI, "KEY_LEFT_GUI"},

                          {KEY_RIGHT_CTRL, "KEY_RIGHT_CTRL"},
                          {KEY_RIGHT_SHIFT, "KEY_RIGHT_SHIFT"},
                          {KEY_RIGHT_ALT, "KEY_RIGHT_ALT"},
                          {KEY_RIGHT_GUI, "KEY_RIGHT_GUI"},

                          {KEY_UP_ARROW, "KEY_UP_ARROW"},
                          {KEY_DOWN_ARROW, "KEY_DOWN_ARROW"},
                          {KEY_LEFT_ARROW, "KEY_LEFT_ARROW"},
                          {KEY_RIGHT_ARROW, "KEY_RIGHT_ARROW"},

                          {KEY_MENU, "KEY_MENU"},
                          {KEY_SPACE, "KEY_SPACE"},
                          {KEY_BACKSPACE, "KEY_BACKSPACE"},
                          {KEY_TAB, "KEY_TAB"},
                          {KEY_RETURN, "KEY_RETURN"},
                          {KEY_ESC, "KEY_ESC"},

                          {KEY_INSERT, "KEY_INSERT"},
                          {KEY_DELETE, "KEY_DELETE"},
                          {KEY_PAGE_UP, "KEY_PAGE_UP"},
                          {KEY_PAGE_DOWN, "KEY_PAGE_DOWN"},
                          {KEY_HOME, "KEY_HOME"},
                          {KEY_END, "KEY_END"},

                          {KEY_NUM_LOCK, "KEY_NUM_LOCK"},
                          {KEY_CAPS_LOCK, "KEY_CAPS_LOCK"},

                          {KEY_F1, "KEY_F1"},
                          {KEY_F2, "KEY_F2"},
                          {KEY_F3, "KEY_F3"},
                          {KEY_F4, "KEY_F4"},
                          {KEY_F5, "KEY_F5"},
                          {KEY_F6, "KEY_F6"},
                          {KEY_F7, "KEY_F7"},
                          {KEY_F8, "KEY_F8"},
                          {KEY_F9, "KEY_F9"},
                          {KEY_F10, "KEY_F10"},
                          {KEY_F11, "KEY_F11"},
                          {KEY_F12, "KEY_F12"},

                          {KEY_F13, "KEY_F13"},
                          {KEY_F14, "KEY_F14"},
                          {KEY_F15, "KEY_F15"},
                          {KEY_F16, "KEY_F16"},
                          {KEY_F17, "KEY_F17"},
                          {KEY_F18, "KEY_F18"},
                          {KEY_F19, "KEY_F19"},
                          {KEY_F20, "KEY_F20"},
                          {KEY_F21, "KEY_F21"},
                          {KEY_F22, "KEY_F22"},
                          {KEY_F23, "KEY_F23"},
                          {KEY_F24, "KEY_F24"},

                          {KEY_PRINT_SCREEN, "KEY_PRINT_SCREEN"},
                          {KEY_SCROLL_LOCK, "KEY_SCROLL_LOCK"},
                          {KEY_PAUSE, "KEY_PAUSE"}};

const int keyMapSize = sizeof(keyMap) / sizeof(keyMap[0]);

const uint8_t buttonPins[NUM_BUTTONS] = {
    BUTTON_PIN_0_P1, BUTTON_PIN_1_P1, BUTTON_PIN_2_P1, BUTTON_PIN_3_P1,
    BUTTON_PIN_4_P1, BUTTON_PIN_5_P1, BUTTON_PIN_6_P1, BUTTON_PIN_7_P1,
    BUTTON_PIN_8_P1, BUTTON_PIN_9_P1, BUTTON_PIN_0_P2, BUTTON_PIN_1_P2,
    BUTTON_PIN_2_P2, BUTTON_PIN_3_P2, BUTTON_PIN_4_P2, BUTTON_PIN_5_P2,
    BUTTON_PIN_6_P2, BUTTON_PIN_7_P2, BUTTON_PIN_8_P2, BUTTON_PIN_9_P2};

// const uint8_t keys[NUM_BUTTONS] = {
//     _KEY_0_P1, _KEY_1_P1, _KEY_2_P1, _KEY_3_P1, _KEY_4_P1, _KEY_5_P1,
//     _KEY_6_P1, _KEY_7_P1, _KEY_8_P1, _KEY_9_P1, _KEY_0_P2, _KEY_1_P2,
//     _KEY_2_P2, _KEY_3_P2, _KEY_4_P2, _KEY_5_P2, _KEY_6_P2, _KEY_7_P2,
//     _KEY_8_P2, _KEY_9_P2,
// };

bool buttonState[NUM_BUTTONS] = {false};

bool probeButton(uint8_t buttonPin, uint8_t buttonNumber,
                 const uint8_t keys[NUM_BUTTONS]) {
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

void startPreferences() { preferences.begin("prefs"); }

void keyboardTask(void *pvParameters) {
  _KEY_0_P1 = preferences.getChar("key-player1-0", 'z');
  _KEY_1_P1 = preferences.getChar("key-player1-1", 'x');
  _KEY_2_P1 = preferences.getChar("key-player1-2", 'q');
  _KEY_3_P1 = preferences.getChar("key-player1-3", 'w');
  _KEY_4_P1 = preferences.getChar("key-player1-4", 'a');
  _KEY_5_P1 = preferences.getChar("key-player1-5", 's');
  _KEY_6_P1 = preferences.getChar("key-player1-6", '1');
  _KEY_7_P1 = preferences.getChar("key-player1-7", '2');
  _KEY_8_P1 = preferences.getChar("key-player1-8", KEY_SPACE);
  _KEY_9_P1 = preferences.getChar("key-player1-9", KEY_RETURN);

  _KEY_0_P2 = preferences.getChar("key-player2-0", 't');
  _KEY_1_P2 = preferences.getChar("key-player2-1", 'y');
  _KEY_2_P2 = preferences.getChar("key-player2-2", 'u');
  _KEY_3_P2 = preferences.getChar("key-player2-3", 'i');
  _KEY_4_P2 = preferences.getChar("key-player2-4", 'o');
  _KEY_5_P2 = preferences.getChar("key-player2-5", 'f');
  _KEY_6_P2 = preferences.getChar("key-player2-6", '3');
  _KEY_7_P2 = preferences.getChar("key-player2-7", '4');
  _KEY_8_P2 = preferences.getChar("key-player2-8", 'g');
  _KEY_9_P2 = preferences.getChar("key-player2-9", 'j');

  const uint8_t keys[NUM_BUTTONS] = {
      _KEY_0_P1, _KEY_1_P1, _KEY_2_P1, _KEY_3_P1, _KEY_4_P1,
      _KEY_5_P1, _KEY_6_P1, _KEY_7_P1, _KEY_8_P1, _KEY_9_P1,
      _KEY_0_P2, _KEY_1_P2, _KEY_2_P2, _KEY_3_P2, _KEY_4_P2,
      _KEY_5_P2, _KEY_6_P2, _KEY_7_P2, _KEY_8_P2, _KEY_9_P2,
  };
  while (1) {
    bool buttonPushed = false;
    if (!pauseProbing) {
      for (int i = 0; i < NUM_BUTTONS; i++) {
        if (probeButton(buttonPins[i], i, keys)) {
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
