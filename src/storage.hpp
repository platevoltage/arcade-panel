#include <Arduino.h>

#include "USB.h"
#include "USBMSC.h"
#include "keyboard.hpp"
#include <ArduinoJson.h>
#include <Preferences.h>

USBMSC MSC;

#define FAT_U8(v) ((v) & 0xFF)
#define FAT_U16(v) FAT_U8(v), FAT_U8((v) >> 8)
#define FAT_U32(v)                                                             \
  FAT_U8(v), FAT_U8((v) >> 8), FAT_U8((v) >> 16), FAT_U8((v) >> 24)
#define FAT_MS2B(s, ms) FAT_U8(((((s) & 0x1) * 1000) + (ms)) / 10)
#define FAT_HMS2B(h, m, s)                                                     \
  FAT_U8(((s) >> 1) | (((m) & 0x7) << 5)),                                     \
      FAT_U8((((m) >> 3) & 0x7) | ((h) << 3))
#define FAT_YMD2B(y, m, d)                                                     \
  FAT_U8(((d) & 0x1F) | (((m) & 0x7) << 5)),                                   \
      FAT_U8((((m) >> 3) & 0x1) | ((((y) - 1980) & 0x7F) << 1))
#define FAT_TBL2B(l, h)                                                        \
  FAT_U8(l), FAT_U8(((l >> 8) & 0xF) | ((h << 4) & 0xF0)), FAT_U8(h >> 4)

// #define README_CONTENTS ""
#define FILE_SIZE 512
static const uint8_t README_CONTENTS[FILE_SIZE] = {0};

// Preferences preferences;
static const uint32_t DISK_SECTOR_COUNT =
    2 * 8; // 8KB is the smallest size that windows allow to mount
static const uint16_t DISK_SECTOR_SIZE = 512; // Should be 512
static const uint16_t DISC_SECTORS_PER_TABLE =
    1; // each table sector can fit 170KB (340 sectors)

static uint8_t msc_disk[DISK_SECTOR_COUNT][DISK_SECTOR_SIZE] = {
    //------------- Block0: Boot Sector -------------//
    {                                        // Header (62 bytes)
     0xEB, 0x3C, 0x90,                       // jump_instruction
     'M', 'S', 'D', 'O', 'S', '5', '.', '0', // oem_name
     FAT_U16(DISK_SECTOR_SIZE),              // bytes_per_sector
     FAT_U8(1),                              // sectors_per_cluster
     FAT_U16(1),                             // reserved_sectors_count
     FAT_U8(1),                              // file_alloc_tables_num
     FAT_U16(16),                            // max_root_dir_entries
     FAT_U16(DISK_SECTOR_COUNT),             // fat12_sector_num
     0xF8,                                   // media_descriptor
     FAT_U16(
         DISC_SECTORS_PER_TABLE), // sectors_per_alloc_table;//FAT12 and FAT16
     FAT_U16(
         1), // sectors_per_track;//A value of 0 may indicate LBA-only access
     FAT_U16(1), // num_heads
     FAT_U32(0), // hidden_sectors_count
     FAT_U32(0), // total_sectors_32
     0x00, // physical_drive_number;0x00 for (first) removable media, 0x80 for
           // (first) fixed disk
     0x00, // reserved
     0x29, // extended_boot_signature;//should be 0x29
     FAT_U32(0x1234), // serial_number: 0x1234 => 1234
     'T', 'i', 'n', 'y', 'U', 'S', 'B', ' ', 'M', 'S',
     'C', // volume_label padded with spaces (0x20)
     'F', 'A', 'T', '1', '2', ' ', ' ',
     ' ', // file_system_type padded with spaces (0x20)

     // Zero up to 2 last bytes of FAT magic code (448 bytes)
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00,

     // boot signature (2 bytes)
     0x55, 0xAA},

    //------------- Block1: FAT12 Table -------------//
    {
        FAT_TBL2B(0xFF8, 0xFFF),
        FAT_TBL2B(0xFFF, 0x000) // first 2 entries must be 0xFF8 0xFFF, third
                                // entry is cluster end of readme file
    },

    //------------- Block2: Root Directory -------------//
    {
        // first entry is volume label
        'A', 'R', 'C', 'A', 'D', 'E', 'P', 'A', 'N', 'E', 'L',
        0x08, // FILE_ATTR_VOLUME_LABEL
        0x00, FAT_MS2B(0, 0), FAT_HMS2B(0, 0, 0), FAT_YMD2B(0, 0, 0),
        FAT_YMD2B(0, 0, 0), FAT_U16(0),
        FAT_HMS2B(13, 42, 30),  // last_modified_hms
        FAT_YMD2B(2018, 11, 5), // last_modified_ymd
        FAT_U16(0), FAT_U32(0),

        // second entry is readme file
        'c', 'o', 'n', 'f', 'i', 'g', ' ',
        ' ',                    // file_name[8]; padded with spaces (0x20)
        'j', 's', 'n',          // file_extension[3]; padded with spaces (0x20)
        0x20,                   // file attributes: FILE_ATTR_ARCHIVE
        0x00,                   // ignore
        FAT_MS2B(1, 980),       // creation_time_10_ms (max 199x10 = 1s 990ms)
        FAT_HMS2B(13, 42, 36),  // create_time_hms [5:6:5] => h:m:(s/2)
        FAT_YMD2B(2018, 11, 5), // create_time_ymd [7:4:5] => (y+1980):m:d
        FAT_YMD2B(2020, 11, 5), // last_access_ymd
        FAT_U16(0),             // extended_attributes
        FAT_HMS2B(13, 44, 16),  // last_modified_hms
        FAT_YMD2B(2019, 11, 5), // last_modified_ymd
        FAT_U16(2),             // start of file in cluster
        FAT_U32(FILE_SIZE)      // file size
    },

    //------------- Block3: Readme Content -------------//
    {0}};

char getValueRead(const char *input) {
  for (int i = 0; i < keyMapSize; i++) {
    if (strcmp(keyMap[i].name, input) == 0) {
      // strcpy(buf, keyMap[i].name);
      return keyMap[i].code;
    }
  }

  // buf[0] = input;
  // buf[1] = '\0';
  return input[0];
}

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer,
                       uint32_t bufsize) {

  memcpy(msc_disk[lba] + offset, buffer, bufsize);

  Serial.printf("WRITE LBA %lu\n", lba);

  // dump whole disk looking for README text
  for (int i = 0; i < DISK_SECTOR_COUNT; i++) {

    char *data = (char *)msc_disk[i];
    data[FILE_SIZE - 1] = '\0';

    if (strstr(data, "\"keys\": {")) {

      Serial.println("CONFIG FOUND:");
      Serial.println(data);

      JsonDocument doc;
      deserializeJson(doc, buffer);

      const char *p1_0 = doc["keys"]["player1"]["0"];
      if (p1_0) {
        char _p1_0 = getValueRead(p1_0);
        preferences.putChar("key-player1-0", _p1_0);
        _KEY_0_P1 = _p1_0;
      }

      const char *p1_1 = doc["keys"]["player1"]["1"];
      if (p1_1) {
        char _p1_1 = getValueRead(p1_1);
        preferences.putChar("key-player1-1", _p1_1);
        _KEY_1_P1 = _p1_1;
      }

      const char *p1_2 = doc["keys"]["player1"]["2"];
      if (p1_2) {
        char _p1_2 = getValueRead(p1_2);
        preferences.putChar("key-player1-2", _p1_2);
        _KEY_2_P1 = _p1_2;
      }

      const char *p1_3 = doc["keys"]["player1"]["3"];
      if (p1_3) {
        char _p1_3 = getValueRead(p1_3);
        preferences.putChar("key-player1-3", _p1_3);
        _KEY_3_P1 = _p1_3;
      }

      const char *p1_4 = doc["keys"]["player1"]["4"];
      if (p1_4) {
        char _p1_4 = getValueRead(p1_4);
        preferences.putChar("key-player1-4", _p1_4);
        _KEY_4_P1 = _p1_4;
      }

      const char *p1_5 = doc["keys"]["player1"]["5"];
      if (p1_5) {
        char _p1_5 = getValueRead(p1_5);
        preferences.putChar("key-player1-5", _p1_5);
        _KEY_5_P1 = _p1_5;
      }

      const char *p1_6 = doc["keys"]["player1"]["6"];
      if (p1_6) {
        char _p1_6 = getValueRead(p1_6);
        preferences.putChar("key-player1-6", _p1_6);
        _KEY_6_P1 = _p1_6;
      }

      const char *p1_7 = doc["keys"]["player1"]["7"];
      if (p1_7) {
        char _p1_7 = getValueRead(p1_7);
        preferences.putChar("key-player1-7", _p1_7);
        _KEY_7_P1 = _p1_7;
      }

      const char *p1_8 = doc["keys"]["player1"]["8"];
      if (p1_8) {
        char _p1_8 = getValueRead(p1_8);
        preferences.putChar("key-player1-8", _p1_8);
        _KEY_8_P1 = _p1_8;
      }

      const char *p1_9 = doc["keys"]["player1"]["9"];
      if (p1_9) {
        char _p1_9 = getValueRead(p1_9);
        preferences.putChar("key-player1-9", _p1_9);
        _KEY_9_P1 = _p1_9;
      }

      const char *p2_0 = doc["keys"]["player2"]["0"];
      if (p2_0) {
        char _p2_0 = getValueRead(p2_0);
        preferences.putChar("key-player2-0", _p2_0);
        _KEY_0_P2 = _p2_0;
      }

      const char *p2_1 = doc["keys"]["player2"]["1"];
      if (p2_1) {
        char _p2_1 = getValueRead(p2_1);
        preferences.putChar("key-player2-1", _p2_1);
        _KEY_1_P2 = _p2_1;
      }

      const char *p2_2 = doc["keys"]["player2"]["2"];
      if (p2_2) {
        char _p2_2 = getValueRead(p2_2);
        preferences.putChar("key-player2-2", _p2_2);
        _KEY_2_P2 = _p2_2;
      }

      const char *p2_3 = doc["keys"]["player2"]["3"];
      if (p2_3) {
        char _p2_3 = getValueRead(p2_3);
        preferences.putChar("key-player2-3", _p2_3);
        _KEY_3_P2 = _p2_3;
      }

      const char *p2_4 = doc["keys"]["player2"]["4"];
      if (p2_4) {
        char _p2_4 = getValueRead(p2_4);
        preferences.putChar("key-player2-4", _p2_4);
        _KEY_4_P2 = _p2_4;
      }

      const char *p2_5 = doc["keys"]["player2"]["5"];
      if (p2_5) {
        char _p2_5 = getValueRead(p2_5);
        preferences.putChar("key-player2-5", _p2_5);
        _KEY_5_P2 = _p2_5;
      }

      const char *p2_6 = doc["keys"]["player2"]["6"];
      if (p2_6) {
        char _p2_6 = getValueRead(p2_6);
        preferences.putChar("key-player2-6", _p2_6);
        _KEY_6_P2 = _p2_6;
      }

      const char *p2_7 = doc["keys"]["player2"]["7"];
      if (p2_7) {
        char _p2_7 = getValueRead(p2_7);
        preferences.putChar("key-player2-7", _p2_7);
        _KEY_7_P2 = _p2_7;
      }

      const char *p2_8 = doc["keys"]["player2"]["8"];
      if (p2_8) {
        char _p2_8 = getValueRead(p2_8);
        preferences.putChar("key-player2-8", _p2_8);
        _KEY_8_P2 = _p2_8;
      }

      const char *p2_9 = doc["keys"]["player2"]["9"];
      if (p2_9) {
        char _p2_9 = getValueRead(p2_9);
        preferences.putChar("key-player2-9", _p2_9);
        _KEY_9_P2 = _p2_9;
      }
    }
  }

  if (millis() > 10000 && keyboardTaskHandle) {
    vTaskDelay(1);
    vTaskDelete(keyboardTaskHandle);
    keyboardTaskHandle = NULL;
  }

  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer,
                      uint32_t bufsize) {
  Serial.printf("MSC READ: lba: %" PRIu32 ", offset: %" PRIu32
                ", bufsize: %" PRIu32 "\n",
                lba, offset, bufsize);
  memcpy(buffer, msc_disk[lba] + offset, bufsize);

  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) {
  Serial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n",
                power_condition, start, load_eject);
  return true;
}

static void usbEventCallback(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id) {
    case ARDUINO_USB_STARTED_EVENT:
      Serial.println("USB PLUGGED");
      break;
    case ARDUINO_USB_STOPPED_EVENT:
      Serial.println("USB UNPLUGGED");
      break;
    case ARDUINO_USB_SUSPEND_EVENT:
      Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n",
                    data->suspend.remote_wakeup_en);
      break;
    case ARDUINO_USB_RESUME_EVENT:
      Serial.println("USB RESUMED");
      break;

    default:
      break;
    }
  }
}

void getValueWrite(char input, char *buf) {
  for (int i = 0; i < keyMapSize; i++) {
    if (input == keyMap[i].code) {
      strcpy(buf, keyMap[i].name);
      return;
    }
  }

  buf[0] = input;
  buf[1] = '\0';
}

void writeFileOnBoot() {
  // char msg[64];
  JsonDocument doc;

  char buf[20] = {' ', '\0'};

  getValueWrite(preferences.getChar("key-player1-0", 'z'), buf);
  doc["keys"]["player1"]["0"] = buf;
  getValueWrite(preferences.getChar("key-player1-1", 'x'), buf);
  doc["keys"]["player1"]["1"] = buf;
  getValueWrite(preferences.getChar("key-player1-2", 'q'), buf);
  doc["keys"]["player1"]["2"] = buf;
  getValueWrite(preferences.getChar("key-player1-3", 'w'), buf);
  doc["keys"]["player1"]["3"] = buf;
  getValueWrite(preferences.getChar("key-player1-4", 'a'), buf);
  doc["keys"]["player1"]["4"] = buf;
  getValueWrite(preferences.getChar("key-player1-5", 's'), buf);
  doc["keys"]["player1"]["5"] = buf;
  getValueWrite(preferences.getChar("key-player1-6", '1'), buf);
  doc["keys"]["player1"]["6"] = buf;
  getValueWrite(preferences.getChar("key-player1-7", '2'), buf);
  doc["keys"]["player1"]["7"] = buf;
  getValueWrite(preferences.getChar("key-player1-8", KEY_SPACE), buf);
  doc["keys"]["player1"]["8"] = buf;
  getValueWrite(preferences.getChar("key-player1-9", KEY_RETURN), buf);
  doc["keys"]["player1"]["9"] = buf;

  getValueWrite(preferences.getChar("key-player2-0", 't'), buf);
  doc["keys"]["player2"]["0"] = buf;
  getValueWrite(preferences.getChar("key-player2-1", 'y'), buf);
  doc["keys"]["player2"]["1"] = buf;
  getValueWrite(preferences.getChar("key-player2-2", 'u'), buf);
  doc["keys"]["player2"]["2"] = buf;
  getValueWrite(preferences.getChar("key-player2-3", 'i'), buf);
  doc["keys"]["player2"]["3"] = buf;
  getValueWrite(preferences.getChar("key-player2-4", 'o'), buf);
  doc["keys"]["player2"]["4"] = buf;
  getValueWrite(preferences.getChar("key-player2-5", 'f'), buf);
  doc["keys"]["player2"]["5"] = buf;
  getValueWrite(preferences.getChar("key-player2-6", '3'), buf);
  doc["keys"]["player2"]["6"] = buf;
  getValueWrite(preferences.getChar("key-player2-7", '4'), buf);
  doc["keys"]["player2"]["7"] = buf;
  getValueWrite(preferences.getChar("key-player2-8", 'g'), buf);
  doc["keys"]["player2"]["8"] = buf;
  getValueWrite(preferences.getChar("key-player2-9", 'j'), buf);
  doc["keys"]["player2"]["9"] = buf;

  const char *blank =
      "                                                                        "
      "                                                                        "
      "                                                                        "
      "                                                                        "
      "                                                                        "
      "                                                                        "
      "                                                                        "
      "                                                                       "
      "                                                                        "
      " ";

  memcpy(msc_disk[3], blank, FILE_SIZE);
  size_t len = serializeJsonPretty(doc, (char *)msc_disk[3], 512);
  msc_disk[3][len] = '\n';
  // memcpy(msc_disk[3], msg, strlen(msg));
}

void storageSetup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  USB.onEvent(usbEventCallback);
  MSC.vendorID("ESP32");      // max 8 chars
  MSC.productID("USB_MSC");   // max 16 chars
  MSC.productRevision("1.0"); // max 4 chars
  MSC.onStartStop(onStartStop);
  MSC.onRead(onRead);
  MSC.onWrite(onWrite);

  MSC.mediaPresent(true);
  MSC.isWritable(true); // true if writable, false if read-only

  MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);
  //   USB.begin();
  // preferences.begin("prefs");
  writeFileOnBoot();
}
