#ifndef STORAGE_H
#define STORAGE_H

#include "Adafruit_TinyUSB.h"
#include "Keyboard.h"
#include "hardware/flash.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "pico/multicore.h"
#include <ArduinoJson.h>

// #include "ramdisk.h"

#define FLASH_TOTAL (2 * 1024 * 1024UL)
#define FS_SIZE (1536 * 1024UL)
#define FS_OFFSET (FLASH_TOTAL - FS_SIZE)
#define BLOCK_SIZE 512
#define BLOCK_COUNT (FS_SIZE / BLOCK_SIZE) // 3072

#define RESERVED_SECTORS 1
#define NUM_FATS 2
#define SECTORS_PER_FAT 12
#define ROOT_ENTRIES 512
#define ROOT_SECTORS ((ROOT_ENTRIES * 32) / BLOCK_SIZE) // 32
#define SECTORS_PER_CLUS 1

#define RAM_DISK_BLOCKS 32 // 16KB, adjust as needed

class Storage {
public:
  // Constructor
  Storage();

  // Destructor
  ~Storage();

  // Member functions
  static void begin();
  static void task();

private:
  static Adafruit_USBD_MSC usb_msc;
  static uint8_t sector_buf[FLASH_SECTOR_SIZE];
  static int32_t msc_read_cb(uint32_t lba, void *buffer, uint32_t bufsize);
  static int32_t msc_write_cb(uint32_t lba, uint8_t *buffer, uint32_t bufsize);
  static void msc_flush_cb(void);
  static bool debug;
  static bool isRam;
  static bool debugLBA;
  static uint32_t last_write_ms;
  static uint16_t fat12_entry(uint8_t *fat, uint16_t cluster);
  static int32_t read_file(const char *filename_8_3, uint8_t *buf,
                           uint32_t maxLen, bool fromRam);
  static void flash_write_block(uint32_t lba, const uint8_t *buf);
  static bool is_formatted();
  static void format_fat16();

  static uint8_t format_buf[BLOCK_SIZE]; // replaces buf in format_fat16
  static uint8_t local_buf[FLASH_SECTOR_SIZE];

  static uint16_t find_free_cluster(uint8_t *fat, uint16_t start_from = 2);
  static void set_fat12_entry(uint8_t *fat, uint16_t cluster, uint16_t value);
  static int32_t write_file(const char *filename_8_3, const uint8_t *buf,
                            uint32_t size);
  static int32_t write_ram_file(const char *filename_8_3, const uint8_t *buf,
                                uint32_t size);
  static void createConfigFile();
  static uint8_t ram_disk[RAM_DISK_BLOCKS * BLOCK_SIZE];
  static int32_t msc_ram_read_cb(uint32_t lba, void *buffer, uint32_t bufsize);
  static int32_t msc_ram_write_cb(uint32_t lba, uint8_t *buffer,
                                  uint32_t bufsize);
  static void msc_ram_flush_cb(void);
  static void format_ram_disk();
};

extern Storage storage;

#endif // STORAGE_H