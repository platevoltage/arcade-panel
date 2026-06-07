#include "Keyboard.h"

Keyboard::Keyboard() {
  // constructor body (can be empty)
}

Keyboard::~Keyboard() {}

Adafruit_USBD_HID Keyboard::keyboard;

uint8_t const Keyboard::desc_hid_report[] = {TUD_HID_REPORT_DESC_KEYBOARD()};

uint8_t Keyboard::pins[] = {D0, D1, D2, D3};
uint8_t Keyboard::pincount = sizeof(pins) / sizeof(pins[0]);
uint8_t Keyboard::hidcode[] = {HID_KEY_0, HID_KEY_1, HID_KEY_2, HID_KEY_3};
bool Keyboard::activeState = false;

void Keyboard::process_hid() {
  // used to avoid send multiple consecutive zero report for keyboard
  static bool keyPressedPreviously = false;

  uint8_t count = 0;
  uint8_t keycode[6] = {0};

  // scan normal key and send report
  for (uint8_t i = 0; i < pincount; i++) {
    if (activeState == digitalRead(pins[i])) {
      // if (activeState == digitalRead(pins[i])) {
      // if pin is active (low), add its hid code to key report
      keycode[count++] = hidcode[i];

      // 6 is max keycode per report
      if (count == 6)
        break;
    }
  }

  if (TinyUSBDevice.suspended() && count) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  // skip if hid is not ready e.g still transferring previous report
  if (!keyboard.ready())
    return;

  if (count) {
    // Send report if there is key pressed
    uint8_t const report_id = 0;
    uint8_t const modifier = 0;

    keyPressedPreviously = true;
    keyboard.keyboardReport(report_id, modifier, keycode);
  } else {
    // Send All-zero report to indicate there is no keys pressed
    // Most of the time, it is, though we don't need to send zero report
    // every loop(), only a key is pressed in previous loop()
    if (keyPressedPreviously) {
      keyPressedPreviously = false;
      keyboard.keyboardRelease(0);
    }
  }
}

void Keyboard::begin() {
  for (uint8_t i = 0; i < pincount; i++) {
    pinMode(pins[i], activeState ? INPUT_PULLDOWN : INPUT_PULLUP);
  }

  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  Serial.println("Keyboard begin");
  keyboard.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  keyboard.setPollInterval(2);
  keyboard.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  keyboard.setStringDescriptor("TinyUSB Keyboard");

  keyboard.begin();
  //
}

void Keyboard::task() {
  if (!TinyUSBDevice.mounted()) {
    return;
  }
  process_hid();
  //
}

const KeyName Keyboard::keyMap[] = {{KEY_LEFT_CTRL, "KEY_LEFT_CTRL"},
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
                                    // {KEY_SPACE, "KEY_SPACE"},
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

const int Keyboard::keyMapSize = sizeof(keyMap) / sizeof(keyMap[0]);
