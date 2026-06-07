#ifndef STORAGE_H
#define STORAGE_H

#define DISK_BLOCK_NUM 16
#define DISK_BLOCK_SIZE 512

#include "Adafruit_TinyUSB.h"
#include "KeyboardEmu.h"
#include <ArduinoJson.h>
// #include "ramdisk.h"

class Storage {
public:
  // Constructor
  Storage();

  // Destructor
  ~Storage();

  // Member functions
  static void begin();
  // static void task();
  static Adafruit_USBD_MSC usb_msc;

private:
  static int32_t msc_read_callback(uint32_t lba, void *buffer,
                                   uint32_t bufsize);
  static int32_t msc_write_callback(uint32_t lba, uint8_t *buffer,
                                    uint32_t bufsize);
  static void msc_flush_callback(void);
  static bool msc_start_stop_callback(uint8_t power_condition, bool start,
                                      bool load_eject);
  static bool msc_ready_callback(void);
  static uint8_t msc_disk[DISK_BLOCK_NUM][DISK_BLOCK_SIZE];

  static void getValueWrite(char input, char *buf);
  static void writeFileOnBoot();
};

extern Storage storage;

#endif // STORAGE_H