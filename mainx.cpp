/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2021 NeKuNeKo for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include "Adafruit_TinyUSB.h"
#include "Gamepad.h"
#include <Arduino.h>

#ifdef CFG_TUSB_CONFIG_FILE
#include CFG_TUSB_CONFIG_FILE
#else
#include "tusb_config_custom.h"
#endif
// 8KB is the smallest size that windows allow to mount
#define DISK_BLOCK_NUM 16
#define DISK_BLOCK_SIZE 512

#include "ramdisk.h"

Adafruit_USBD_MSC usb_msc;

// Eject button to demonstrate medium is not ready e.g SDCard is not present
// whenever this button is pressed and hold, it will report to host as not ready
#if defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) ||                         \
    defined(ARDUINO_NRF52840_CIRCUITPLAY)
#define BTN_EJECT 4 // Left Button
bool activeState = true;

#elif defined(ARDUINO_FUNHOUSE_ESP32S2)
#define BTN_EJECT BUTTON_DOWN
bool activeState = true;

#elif defined PIN_BUTTON1
#define BTN_EJECT PIN_BUTTON1
bool activeState = false;
#endif

int32_t msc_read_callback(uint32_t lba, void *buffer, uint32_t bufsize);
int32_t msc_write_callback(uint32_t lba, uint8_t *buffer, uint32_t bufsize);
void msc_flush_callback(void);
bool msc_start_stop_callback(uint8_t power_condition, bool start,
                             bool load_eject);
bool msc_ready_callback(void);

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {TUD_HID_REPORT_DESC_GAMEPAD()};
uint8_t const desc_hid_report_keyboard[] = {TUD_HID_REPORT_DESC_KEYBOARD()};
// USB HID object
// Adafruit_USBD_HID usb_hid;
// Adafruit_USBD_HID usb_hid1;

// Report payload defined in src/class/hid/hid.h
// - For Gamepad Button Bit Mask see  hid_gamepad_button_bm_t
// - For Gamepad Hat    Bit Mask see  hid_gamepad_hat_t
hid_gamepad_report_t gp;

Adafruit_USBD_HID usb_hid_keyboard;

//------------- Input Pins -------------//
// Array of pins and its keycode.
// Notes: these pins can be replaced by PIN_BUTTONn if defined in setup()
#ifdef ARDUINO_ARCH_RP2040
uint8_t pins[] = {D0, D1, D2, D3};
#else
uint8_t pins[] = {A0, A1, A2, A3};
#endif

// number of pins
uint8_t pincount = sizeof(pins) / sizeof(pins[0]);

// For keycode definition check out
// https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
uint8_t hidcode[] = {HID_KEY_0, HID_KEY_1, HID_KEY_2, HID_KEY_3};

#if defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) ||                         \
    defined(ARDUINO_NRF52840_CIRCUITPLAY) || defined(ARDUINO_FUNHOUSE_ESP32S2)
bool activeState = true;
#else
bool activeState = false;
#endif

void setup() {
  // Manual begin() is required on core without built-in support e.g. mbed
  // rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  Serial.begin(115200);

  // Setup HID
  player2.setPollInterval(2);
  player2.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  player2.setStringDescriptor("FUCfdsaK");
  player2.begin();

  player1.setPollInterval(2);
  player1.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  player1.setStringDescriptor("FUCK");
  player1.begin();

  usb_hid_keyboard.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid_keyboard.setPollInterval(2);
  usb_hid_keyboard.setReportDescriptor(desc_hid_report_keyboard,
                                       sizeof(desc_hid_report_keyboard));
  usb_hid_keyboard.setStringDescriptor("TinyUSB Keyboard");

  usb_hid_keyboard.begin();
  // Set disk vendor id, product id and revision with string up to 8, 16, 4
  // characters respectively
  usb_msc.setID("Adafruit", "Mass Storage", "1.0");

  // Set disk size
  usb_msc.setCapacity(DISK_BLOCK_NUM, DISK_BLOCK_SIZE);

  // Set callback
  usb_msc.setReadWriteCallback(msc_read_callback, msc_write_callback,
                               msc_flush_callback);
  usb_msc.setStartStopCallback(msc_start_stop_callback);
  usb_msc.setReadyCallback(msc_ready_callback);

  // Set Lun ready (RAM disk is always ready)
  usb_msc.setUnitReady(true);
  usb_msc.begin();

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi
  // won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // led pin
#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  // overwrite input pin with PIN_BUTTONx
#ifdef PIN_BUTTON1
  pins[0] = PIN_BUTTON1;
#endif

#ifdef PIN_BUTTON2
  pins[1] = PIN_BUTTON2;
#endif

#ifdef PIN_BUTTON3
  pins[2] = PIN_BUTTON3;
#endif

#ifdef PIN_BUTTON4
  pins[3] = PIN_BUTTON4;
#endif

  // Set up pin as input
  for (uint8_t i = 0; i < pincount; i++) {
    pinMode(pins[i], activeState ? INPUT_PULLDOWN : INPUT_PULLUP);
  }
}

void process_hid() {
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
  if (!usb_hid_keyboard.ready())
    return;

  if (count) {
    // Send report if there is key pressed
    uint8_t const report_id = 0;
    uint8_t const modifier = 0;

    keyPressedPreviously = true;
    usb_hid_keyboard.keyboardReport(report_id, modifier, keycode);
  } else {
    // Send All-zero report to indicate there is no keys pressed
    // Most of the time, it is, though we don't need to send zero report
    // every loop(), only a key is pressed in previous loop()
    if (keyPressedPreviously) {
      keyPressedPreviously = false;
      usb_hid_keyboard.keyboardRelease(0);
    }
  }
}

void loop() {
#ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
#endif

  process_hid();

  // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted()) {
    return;
  }

  //  // Remote wakeup
  //  if ( TinyUSBDevice.suspended() && btn )
  //  {
  //    // Wake up host if we are in suspend mode
  //    // and REMOTE_WAKEUP feature is enabled by host
  //    TinyUSBDevice.remoteWakeup();
  //  }

  if (!player1.ready())
    return;

  // poll gpio once each 2 ms
  // static uint32_t ms = 0;
  // if (millis() - ms > 2) {
  //   ms = millis();
  // }

  // Reset buttons
  Serial.println("No pressing buttons");
  gp.x = 0;
  gp.y = 0;
  gp.z = 0;
  gp.rz = 0;
  gp.rx = 0;
  gp.ry = 0;
  gp.hat = 0;
  gp.buttons = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  usb_hid1.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  Serial.println(CFG_TUD_HID);

  // Hat/DPAD UP
  Serial.println("Hat/DPAD UP");
  gp.hat = 1; // GAMEPAD_HAT_UP;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Hat/DPAD UP RIGHT
  Serial.println("Hat/DPAD UP RIGHT");
  gp.hat = 2; // GAMEPAD_HAT_UP_RIGHT;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Hat/DPAD RIGHT
  Serial.println("Hat/DPAD RIGHT");
  gp.hat = 3; // GAMEPAD_HAT_RIGHT;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Hat/DPAD DOWN RIGHT
  Serial.println("Hat/DPAD DOWN RIGHT");
  gp.hat = 4; // GAMEPAD_HAT_DOWN_RIGHT;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Hat/DPAD DOWN
  Serial.println("Hat/DPAD DOWN");
  gp.hat = 5; // GAMEPAD_HAT_DOWN;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Hat/DPAD DOWN LEFT
  Serial.println("Hat/DPAD DOWN LEFT");
  gp.hat = 6; // GAMEPAD_HAT_DOWN_LEFT;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Hat/DPAD LEFT
  Serial.println("Hat/DPAD LEFT");
  gp.hat = 7; // GAMEPAD_HAT_LEFT;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Hat/DPAD UP LEFT
  Serial.println("Hat/DPAD UP LEFT");
  gp.hat = 8; // GAMEPAD_HAT_UP_LEFT;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Hat/DPAD CENTER
  Serial.println("Hat/DPAD CENTER");
  gp.hat = 0; // GAMEPAD_HAT_CENTERED;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 1 UP
  Serial.println("Joystick 1 UP");
  gp.x = 0;
  gp.y = -127;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 1 DOWN
  Serial.println("Joystick 1 DOWN");
  gp.x = 0;
  gp.y = 127;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 1 RIGHT
  Serial.println("Joystick 1 RIGHT");
  gp.x = 127;
  gp.y = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 1 LEFT
  Serial.println("Joystick 1 LEFT");
  gp.x = -127;
  gp.y = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 1 CENTER
  Serial.println("Joystick 1 CENTER");
  gp.x = 0;
  gp.y = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 2 UP
  Serial.println("Joystick 2 UP");
  gp.z = 0;
  gp.rz = 127;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 2 DOWN
  Serial.println("Joystick 2 DOWN");
  gp.z = 0;
  gp.rz = -127;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 2 RIGHT
  Serial.println("Joystick 2 RIGHT");
  gp.z = 127;
  gp.rz = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 2 LEFT
  Serial.println("Joystick 2 LEFT");
  gp.z = -127;
  gp.rz = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Joystick 2 CENTER
  Serial.println("Joystick 2 CENTER");
  gp.z = 0;
  gp.rz = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Analog Trigger 1 UP
  Serial.println("Analog Trigger 1 UP");
  gp.rx = 127;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Analog Trigger 1 DOWN
  Serial.println("Analog Trigger 1 DOWN");
  gp.rx = -127;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Analog Trigger 1 CENTER
  Serial.println("Analog Trigger 1 CENTER");
  gp.rx = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Analog Trigger 2 UP
  Serial.println("Analog Trigger 2 UP");
  gp.ry = 127;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Analog Trigger 2 DOWN
  Serial.println("Analog Trigger 2 DOWN");
  gp.ry = -127;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Analog Trigger 2 CENTER
  Serial.println("Analog Trigger 2 CENTER");
  gp.ry = 0;
  usb_hid.sendReport(0, &gp, sizeof(gp));
  delay(2000);

  // Test buttons (up to 32 buttons)
  for (int i = 0; i < 32; ++i) {
    Serial.print("Pressing button ");
    Serial.println(i);
    gp.buttons = (1U << i);
    usb_hid.sendReport(0, &gp, sizeof(gp));
    delay(1000);
  }

  // Random touch
  Serial.println("Random touch");
  gp.x = random(-127, 128);
  gp.y = random(-127, 128);
  gp.z = random(-127, 128);
  gp.rz = random(-127, 128);
  gp.rx = random(-127, 128);
  gp.ry = random(-127, 128);
  gp.hat = random(0, 9);
  gp.buttons = random(0, 0xffff);
  usb_hid.sendReport(0, &gp, sizeof(gp));
  usb_hid1.sendReport(0, &gp, sizeof(gp));
  delay(2000);
  // */
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and
// return number of copied bytes (must be multiple of block size)
int32_t msc_read_callback(uint32_t lba, void *buffer, uint32_t bufsize) {
  uint8_t const *addr = msc_disk[lba];
  memcpy(buffer, addr, bufsize);

  return bufsize;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and
// return number of written bytes (must be multiple of block size)
int32_t msc_write_callback(uint32_t lba, uint8_t *buffer, uint32_t bufsize) {
  uint8_t *addr = msc_disk[lba];
  memcpy(addr, buffer, bufsize);

  return bufsize;
}

// Callback invoked when WRITE10 command is completed (status received and
// accepted by host). used to flush any pending cache.
void msc_flush_callback(void) {
  // nothing to do
}

bool msc_start_stop_callback(uint8_t power_condition, bool start,
                             bool load_eject) {
  Serial.printf(
      "Start/Stop callback: power condition %u, start %u, load_eject %u\n",
      power_condition, start, load_eject);
  return true;
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool msc_ready_callback(void) {
#ifdef BTN_EJECT
  // button not active --> medium ready
  return digitalRead(BTN_EJECT) != activeState;
#else
  return true;
#endif
}

// Output report callback for LED indicator such as Caplocks
void hid_report_callback(uint8_t report_id, hid_report_type_t report_type,
                         uint8_t const *buffer, uint16_t bufsize) {
  (void)report_id;
  (void)bufsize;

  // LED indicator is output report with only 1 byte length
  if (report_type != HID_REPORT_TYPE_OUTPUT)
    return;

  // The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
  // Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
  uint8_t ledIndicator = buffer[0];

#ifdef LED_BUILTIN
  // turn on LED if capslock is set
  digitalWrite(LED_BUILTIN, ledIndicator & KEYBOARD_LED_CAPSLOCK);
#endif
}