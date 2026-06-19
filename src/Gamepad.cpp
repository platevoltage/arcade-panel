#include "Gamepad.h"

Gamepad::Gamepad() {
  // constructor body (can be empty)
}

Gamepad::~Gamepad() {}

// Adafruit_USBD_HID Gamepad::player1;
// Adafruit_USBD_HID Gamepad::player2;

Gamepad player1;
Gamepad player2;

// hid_gamepad_report_t Gamepad::gp;
// hid_gamepad_report_t gp2;

uint8_t const Gamepad::desc_hid_report[] = {TUD_HID_REPORT_DESC_GAMEPAD()};

void Gamepad::begin() {
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  Serial.println("Gamepad begin");

  // player2.setPollInterval(2);
  // player2.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  // player2.begin();

  player.setPollInterval(2);
  player.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  player.begin();

  //
}

void Gamepad::task() {
  if (!player.ready())
    return;
  // Serial.println("Gamepad task");
  // Serial.println("No pressing buttons");
  // gp.x = analogX;
  // gp.y = analogY;
  // gp.z = 0;
  // gp.rz = 0;
  // gp.rx = 0;
  // gp.ry = 0;
  // gp.hat = 0;
  // gp.buttons = 0;
  player.sendReport(0, &gp, sizeof(gp));
  delay(20);
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Hat/DPAD UP
  // Serial.println("Hat/DPAD UP");
  // gp.hat = 1; // GAMEPAD_HAT_UP;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Hat/DPAD UP RIGHT
  // Serial.println("Hat/DPAD UP RIGHT");
  // gp.hat = 2; // GAMEPAD_HAT_UP_RIGHT;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Hat/DPAD RIGHT
  // Serial.println("Hat/DPAD RIGHT");
  // gp.hat = 3; // GAMEPAD_HAT_RIGHT;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Hat/DPAD DOWN RIGHT
  // Serial.println("Hat/DPAD DOWN RIGHT");
  // gp.hat = 4; // GAMEPAD_HAT_DOWN_RIGHT;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Hat/DPAD DOWN
  // Serial.println("Hat/DPAD DOWN");
  // gp.hat = 5; // GAMEPAD_HAT_DOWN;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Hat/DPAD DOWN LEFT
  // Serial.println("Hat/DPAD DOWN LEFT");
  // gp.hat = 6; // GAMEPAD_HAT_DOWN_LEFT;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Hat/DPAD LEFT
  // Serial.println("Hat/DPAD LEFT");
  // gp.hat = 7; // GAMEPAD_HAT_LEFT;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Hat/DPAD UP LEFT
  // Serial.println("Hat/DPAD UP LEFT");
  // gp.hat = 8; // GAMEPAD_HAT_UP_LEFT;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Hat/DPAD CENTER
  // Serial.println("Hat/DPAD CENTER");
  // gp.hat = 0; // GAMEPAD_HAT_CENTERED;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 1 UP
  // Serial.println("Joystick 1 UP");
  // gp.x = 0;
  // gp.y = -127;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 1 DOWN
  // Serial.println("Joystick 1 DOWN");
  // gp.x = 0;
  // gp.y = 127;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 1 RIGHT
  // Serial.println("Joystick 1 RIGHT");
  // gp.x = 127;
  // gp.y = 0;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 1 LEFT
  // Serial.println("Joystick 1 LEFT");
  // gp.x = -127;
  // gp.y = 0;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 1 CENTER
  // Serial.println("Joystick 1 CENTER");
  // gp.x = 0;
  // gp.y = 0;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 2 UP
  // Serial.println("Joystick 2 UP");
  // gp.z = 0;
  // gp.rz = 127;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 2 DOWN
  // Serial.println("Joystick 2 DOWN");
  // gp.z = 0;
  // gp.rz = -127;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 2 RIGHT
  // Serial.println("Joystick 2 RIGHT");
  // gp.z = 127;
  // gp.rz = 0;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 2 LEFT
  // Serial.println("Joystick 2 LEFT");
  // gp.z = -127;
  // gp.rz = 0;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Joystick 2 CENTER
  // Serial.println("Joystick 2 CENTER");
  // gp.z = 0;
  // gp.rz = 0;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Analog Trigger 1 UP
  // Serial.println("Analog Trigger 1 UP");
  // gp.rx = 127;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Analog Trigger 1 DOWN
  // Serial.println("Analog Trigger 1 DOWN");
  // gp.rx = -127;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Analog Trigger 1 CENTER
  // Serial.println("Analog Trigger 1 CENTER");
  // gp.rx = 0;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Analog Trigger 2 UP
  // Serial.println("Analog Trigger 2 UP");
  // gp.ry = 127;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Analog Trigger 2 DOWN
  // Serial.println("Analog Trigger 2 DOWN");
  // gp.ry = -127;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Analog Trigger 2 CENTER
  // Serial.println("Analog Trigger 2 CENTER");
  // gp.ry = 0;
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));

  // // Test buttons (up to 32 buttons)
  // for (int i = 0; i < 32; ++i) {
  //   Serial.print("Pressing button ");
  //   Serial.println(i);
  //   gp.buttons = (1U << i);
  //   player1.sendReport(0, &gp, sizeof(gp));
  //   player2.sendReport(0, &gp, sizeof(gp));
  //   delay(10);
  // }

  // // Random touch
  // Serial.println("Random touch");
  // gp.x = random(-127, 128);
  // gp.y = random(-127, 128);
  // gp.z = random(-127, 128);
  // gp.rz = random(-127, 128);
  // gp.rx = random(-127, 128);
  // gp.ry = random(-127, 128);
  // gp.hat = random(0, 9);
  // gp.buttons = random(0, 0xffff);
  // player1.sendReport(0, &gp, sizeof(gp));
  // player2.sendReport(0, &gp, sizeof(gp));
}