#include "Storage.h"

Storage::Storage() {
  // constructor body (can be empty)
}

Storage::~Storage() {}

Adafruit_USBD_MSC Storage::usb_msc;

// #define FLASH_TOTAL (2 * 1024 * 1024UL)
// #define FS_SIZE (1536 * 1024UL)
// #define FS_OFFSET (FLASH_TOTAL - FS_SIZE)
// #define BLOCK_SIZE 512
// #define BLOCK_COUNT (FS_SIZE / BLOCK_SIZE) // 3072

// #define RESERVED_SECTORS 1
// #define NUM_FATS 2
// #define SECTORS_PER_FAT 12
// #define ROOT_ENTRIES 512
// #define ROOT_SECTORS ((ROOT_ENTRIES * 32) / BLOCK_SIZE) // 32
// #define SECTORS_PER_CLUS 1

uint8_t Storage::sector_buf[FLASH_SECTOR_SIZE];
// int32_t Storage::msc_read_cb(uint32_t lba, void *buffer, uint32_t bufsize);
// int32_t Storage::msc_write_cb(uint32_t lba, uint8_t *buffer, uint32_t
// bufsize); void Storage::msc_flush_cb(void);

bool Storage::debug = false;
bool Storage::debugLBA = 0;
uint32_t Storage::last_write_ms = 0;

uint8_t Storage::format_buf[BLOCK_SIZE]; // replaces buf in format_fat16
uint8_t Storage::local_buf[FLASH_SECTOR_SIZE];

#define DATA_START                                                             \
  (RESERVED_SECTORS + NUM_FATS * SECTORS_PER_FAT + ROOT_SECTORS)

// Find a free cluster starting from cluster 2
uint16_t Storage::find_free_cluster(uint8_t *fat, uint16_t start_from) {
  for (uint16_t c = start_from; c < BLOCK_COUNT; c++) {
    if (fat12_entry(fat, c) == 0x000)
      return c;
  }
  return 0; // full
}

// Write a FAT12 entry
void Storage::set_fat12_entry(uint8_t *fat, uint16_t cluster, uint16_t value) {
  uint32_t offset = cluster + (cluster / 2);
  if (cluster & 1) {
    fat[offset] = (fat[offset] & 0x0F) | ((value & 0x0F) << 4);
    fat[offset + 1] = (value >> 4) & 0xFF;
  } else {
    fat[offset] = value & 0xFF;
    fat[offset + 1] = (fat[offset + 1] & 0xF0) | ((value >> 8) & 0x0F);
  }
}

int32_t Storage::write_file(const char *filename_8_3, const uint8_t *buf,
                            uint32_t size) {
  // Load FAT into RAM so we can modify it
  static uint8_t fat_buf[SECTORS_PER_FAT * BLOCK_SIZE];
  memcpy(fat_buf,
         (void *)(XIP_BASE + FS_OFFSET + RESERVED_SECTORS * BLOCK_SIZE),
         SECTORS_PER_FAT * BLOCK_SIZE);

  // Allocate clusters and build chain
  uint32_t clusters_needed = (size + SECTORS_PER_CLUS * BLOCK_SIZE - 1) /
                             (SECTORS_PER_CLUS * BLOCK_SIZE);
  uint16_t first_cluster = 0;
  uint16_t prev_cluster = 0;

  for (uint32_t i = 0; i < clusters_needed; i++) {
    uint16_t c =
        find_free_cluster(fat_buf, prev_cluster ? prev_cluster + 1 : 2);
    if (c == 0) {
      Serial.println("Disk full");
      return -1;
    }
    if (i == 0)
      first_cluster = c;
    if (prev_cluster)
      set_fat12_entry(fat_buf, prev_cluster, c);
    set_fat12_entry(fat_buf, c, 0xFFF); // end of chain for now
    prev_cluster = c;
  }

  // Write data clusters
  uint32_t remaining = size;
  uint16_t cluster = first_cluster;
  uint32_t buf_offset = 0;
  while (remaining > 0) {
    uint32_t sector = DATA_START + (cluster - 2) * SECTORS_PER_CLUS;
    uint8_t block[BLOCK_SIZE];
    uint32_t chunkSize = min(remaining, (uint32_t)BLOCK_SIZE);
    memcpy(block, buf + buf_offset, chunkSize);
    if (chunkSize < BLOCK_SIZE)
      memset(block + chunkSize, 0, BLOCK_SIZE - chunkSize);
    flash_write_block(sector, block);
    buf_offset += chunkSize;
    remaining -= chunkSize;
    cluster = fat12_entry(fat_buf, cluster);
  }

  // Write FAT1 and FAT2 back to flash
  for (int i = 0; i < SECTORS_PER_FAT; i++) {
    flash_write_block(RESERVED_SECTORS + i, fat_buf + i * BLOCK_SIZE);
    flash_write_block(RESERVED_SECTORS + SECTORS_PER_FAT + i,
                      fat_buf + i * BLOCK_SIZE);
  }

  // Find empty directory slot and write entry
  uint32_t root_start = RESERVED_SECTORS + NUM_FATS * SECTORS_PER_FAT;
  for (int e = 0; e < ROOT_ENTRIES; e++) {
    uint32_t entry_sector = root_start + (e * 32) / BLOCK_SIZE;
    uint32_t entry_offset = (e * 32) % BLOCK_SIZE;

    uint8_t *entry_ptr = (uint8_t *)(XIP_BASE + FS_OFFSET +
                                     entry_sector * BLOCK_SIZE + entry_offset);
    if (entry_ptr[0] == 0x00 || entry_ptr[0] == 0xE5) {
      // Read the sector, patch in the entry, write it back
      uint8_t dir_block[BLOCK_SIZE];
      memcpy(dir_block,
             (void *)(XIP_BASE + FS_OFFSET + entry_sector * BLOCK_SIZE),
             BLOCK_SIZE);
      uint8_t *entry = dir_block + entry_offset;

      memcpy(entry, filename_8_3, 11);
      entry[11] = 0x20; // ATTR_ARCHIVE
      memset(entry + 12, 0, 20);

      uint16_t date = ((2025 - 1980) << 9) | (1 << 5) | 1; // 2025-01-01
      uint16_t time = (12 << 11) | (0 << 5) | 0;           // 12:00:00

      entry[22] = time & 0xFF;
      entry[23] = (time >> 8) & 0xFF;
      entry[24] = date & 0xFF;
      entry[25] = (date >> 8) & 0xFF;
      entry[26] = first_cluster & 0xFF;
      entry[27] = (first_cluster >> 8) & 0xFF;
      entry[28] = size & 0xFF;
      entry[29] = (size >> 8) & 0xFF;
      entry[30] = (size >> 16) & 0xFF;
      entry[31] = (size >> 24) & 0xFF;

      flash_write_block(entry_sector, dir_block);
      return size;
    }
  }

  Serial.println("Root directory full");
  return -1;
}

uint16_t Storage::fat12_entry(uint8_t *fat, uint16_t cluster) {
  uint32_t offset = cluster + (cluster / 2);
  uint16_t val = fat[offset] | (fat[offset + 1] << 8);
  if (cluster & 1)
    return val >> 4;
  else
    return val & 0x0FFF;
}

int32_t Storage::read_file(const char *filename_8_3, uint8_t *buf,
                           uint32_t maxLen) {
  uint32_t root_start = RESERVED_SECTORS + NUM_FATS * SECTORS_PER_FAT;
  uint8_t *root = (uint8_t *)(XIP_BASE + FS_OFFSET + root_start * BLOCK_SIZE);
  uint8_t *fat =
      (uint8_t *)(XIP_BASE + FS_OFFSET + RESERVED_SECTORS * BLOCK_SIZE);

  for (int e = 0; e < ROOT_ENTRIES; e++) {
    uint8_t *entry = root + e * 32;
    if (entry[0] == 0x00)
      break;
    if (entry[0] == 0xE5)
      continue;
    if (entry[11] & 0x08)
      continue;
    if (entry[11] & 0x10)
      continue;

    if (memcmp(entry, filename_8_3, 11) == 0) {
      uint16_t cluster = entry[26] | (entry[27] << 8);
      uint32_t size =
          entry[28] | (entry[29] << 8) | (entry[30] << 16) | (entry[31] << 24);

      uint32_t toRead = min(size, maxLen);
      uint32_t offset = 0;

      while (offset < toRead) {
        if (cluster < 2 || cluster >= 0xFF8)
          break;

        uint32_t sector = DATA_START + (cluster - 2) * SECTORS_PER_CLUS;
        uint8_t *src = (uint8_t *)(XIP_BASE + FS_OFFSET + sector * BLOCK_SIZE);
        uint32_t chunkSize =
            min((uint32_t)(SECTORS_PER_CLUS * BLOCK_SIZE), toRead - offset);
        memcpy(buf + offset, src, chunkSize);
        offset += chunkSize;

        cluster = fat12_entry(fat, cluster);
      }

      return size;
    }
  }
  return -1;
}

void Storage::flash_write_block(uint32_t lba, const uint8_t *buf) {
  uint32_t flash_addr = FS_OFFSET + lba * BLOCK_SIZE;
  uint32_t sector_addr = flash_addr & ~(FLASH_SECTOR_SIZE - 1);
  uint32_t offset = flash_addr - sector_addr;

  // uint8_t local_buf[FLASH_SECTOR_SIZE]; // <-- local, not global
  memcpy(local_buf, (void *)(XIP_BASE + sector_addr), FLASH_SECTOR_SIZE);
  memcpy(local_buf + offset, buf, BLOCK_SIZE);

  rp2040.idleOtherCore();
  irq_set_enabled(USBCTRL_IRQ, false);
  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(sector_addr, FLASH_SECTOR_SIZE);
  flash_range_program(sector_addr, local_buf, FLASH_SECTOR_SIZE);
  restore_interrupts(ints);
  irq_set_enabled(USBCTRL_IRQ, true);
  rp2040.resumeOtherCore();
}

bool Storage::is_formatted() {
  uint8_t *p = (uint8_t *)(XIP_BASE + FS_OFFSET);
  return p[510] == 0x55 && p[511] == 0xAA;
}

void Storage::createConfigFile() {
  // const char *msg = "{}";
  JsonDocument doc;

  char buf[20] = {' ', '\0'};

  keyboard.getValueWrite('z', buf);
  doc["keys"]["player1"]["0"] = buf;
  keyboard.getValueWrite('x', buf);
  doc["keys"]["player1"]["1"] = buf;
  keyboard.getValueWrite('q', buf);
  doc["keys"]["player1"]["2"] = buf;
  keyboard.getValueWrite('w', buf);
  doc["keys"]["player1"]["3"] = buf;
  keyboard.getValueWrite('a', buf);
  doc["keys"]["player1"]["4"] = buf;
  keyboard.getValueWrite('s', buf);
  doc["keys"]["player1"]["5"] = buf;
  keyboard.getValueWrite('1', buf);
  doc["keys"]["player1"]["6"] = buf;
  keyboard.getValueWrite('2', buf);
  doc["keys"]["player1"]["7"] = buf;
  keyboard.getValueWrite(KEY_SPACE, buf);
  doc["keys"]["player1"]["8"] = buf;
  keyboard.getValueWrite(KEY_RETURN, buf);
  doc["keys"]["player1"]["9"] = buf;

  keyboard.getValueWrite('t', buf);
  doc["keys"]["player2"]["0"] = buf;
  keyboard.getValueWrite('y', buf);
  doc["keys"]["player2"]["1"] = buf;
  keyboard.getValueWrite('u', buf);
  doc["keys"]["player2"]["2"] = buf;
  keyboard.getValueWrite('i', buf);
  doc["keys"]["player2"]["3"] = buf;
  keyboard.getValueWrite('o', buf);
  doc["keys"]["player2"]["4"] = buf;
  keyboard.getValueWrite('f', buf);
  doc["keys"]["player2"]["5"] = buf;
  keyboard.getValueWrite('3', buf);
  doc["keys"]["player2"]["6"] = buf;
  keyboard.getValueWrite('4', buf);
  doc["keys"]["player2"]["7"] = buf;
  keyboard.getValueWrite('g', buf);
  doc["keys"]["player2"]["8"] = buf;
  keyboard.getValueWrite('j', buf);
  doc["keys"]["player2"]["9"] = buf;

  char jsonBuf[2048];
  size_t jsonLen = serializeJsonPretty(doc, jsonBuf);
  write_file("CONFIG  JSN", (uint8_t *)jsonBuf, strlen(jsonBuf));
}

void Storage::format_fat16() {
  // uint8_t buf[BLOCK_SIZE];
  Serial.println("entered format_fat16");
  // --- Block 0: Boot sector ---
  memset(format_buf, 0, BLOCK_SIZE);
  format_buf[0] = 0xEB;
  format_buf[1] = 0x3C;
  format_buf[2] = 0x90;
  format_buf[3] = 'M';
  format_buf[4] = 'S';
  format_buf[5] = 'D';
  format_buf[6] = 'O';
  format_buf[7] = 'S';
  format_buf[8] = '5';
  format_buf[9] = '.';
  format_buf[10] = '0';
  format_buf[11] = 0x00;
  format_buf[12] = 0x02;             // bytes/sector = 512
  format_buf[13] = SECTORS_PER_CLUS; // sectors/cluster
  format_buf[14] = RESERVED_SECTORS;
  format_buf[15] = 0x00;                       // reserved sectors
  format_buf[16] = NUM_FATS;                   // num FATs
  format_buf[17] = (ROOT_ENTRIES & 0xFF);      // root entries lo
  format_buf[18] = (ROOT_ENTRIES >> 8) & 0xFF; // root entries hi
  format_buf[19] = (BLOCK_COUNT & 0xFF);       // total sectors lo
  format_buf[20] = (BLOCK_COUNT >> 8) & 0xFF;  // total sectors hi
  format_buf[21] = 0xF8;                       // media type
  format_buf[22] = SECTORS_PER_FAT;
  format_buf[23] = 0x00; // sectors/FAT
  format_buf[24] = 0x01;
  format_buf[25] = 0x00; // sectors/track
  format_buf[26] = 0x01;
  format_buf[27] = 0x00; // heads
  format_buf[28] = 0x00;
  format_buf[29] = 0x00;
  format_buf[30] = 0x00;
  format_buf[31] = 0x00; // hidden sectors
  format_buf[32] = 0x00;
  format_buf[33] = 0x00;
  format_buf[34] = 0x00;
  format_buf[35] = 0x00; // total sectors 32
  format_buf[36] = 0x80; // drive number
  format_buf[37] = 0x00;
  format_buf[38] = 0x29; // ext boot sig
  format_buf[39] = 0x34;
  format_buf[40] = 0x12;
  format_buf[41] = 0x00;
  format_buf[42] = 0x00; // volume serial
  format_buf[43] = 'P';
  format_buf[44] = 'I';
  format_buf[45] = 'C';
  format_buf[46] = 'O';
  format_buf[47] = ' ';
  format_buf[48] = 'F';
  format_buf[49] = 'L';
  format_buf[50] = 'A';
  format_buf[51] = 'S';
  format_buf[52] = 'H';
  format_buf[53] = ' ';
  format_buf[54] = 'F';
  format_buf[55] = 'A';
  format_buf[56] = 'T';
  format_buf[57] = '1';
  format_buf[58] = '6';
  format_buf[59] = ' ';
  format_buf[60] = ' ';
  format_buf[61] = ' ';
  format_buf[510] = 0x55;
  format_buf[511] = 0xAA;
  flash_write_block(0, format_buf);

  // --- FAT1 (blocks 1-12) ---
  memset(format_buf, 0, BLOCK_SIZE);
  format_buf[0] = 0xF8;
  format_buf[1] = 0xFF; // media descriptor
  format_buf[2] = 0xFF;
  format_buf[3] = 0xFF; // end of chain
  flash_write_block(1, format_buf);
  memset(format_buf, 0, BLOCK_SIZE);
  for (int i = 2; i <= SECTORS_PER_FAT; i++) {
    flash_write_block(i, format_buf);
  }

  // --- FAT2 (blocks 13-24) ---
  memset(format_buf, 0, BLOCK_SIZE);
  format_buf[0] = 0xF8;
  format_buf[1] = 0xFF;
  format_buf[2] = 0xFF;
  format_buf[3] = 0xFF;
  flash_write_block(RESERVED_SECTORS + SECTORS_PER_FAT, format_buf);
  memset(format_buf, 0, BLOCK_SIZE);
  for (int i = 1; i < SECTORS_PER_FAT; i++) {
    flash_write_block(RESERVED_SECTORS + SECTORS_PER_FAT + i, format_buf);
  }

  // --- Root directory (blocks 25-56) ---
  // First block has volume label entry
  memset(format_buf, 0, BLOCK_SIZE);
  format_buf[0] = 'L';
  format_buf[1] = 'A';
  format_buf[2] = 'I';
  format_buf[3] = 'K';
  format_buf[4] = 'A';
  format_buf[5] = ' ';
  format_buf[6] = ' ';
  format_buf[7] = ' ';
  format_buf[8] = ' ';
  format_buf[9] = ' ';
  format_buf[10] = ' ';
  format_buf[11] = 0x08; // ATTR_VOLUME_ID
  uint32_t root_start = RESERVED_SECTORS + NUM_FATS * SECTORS_PER_FAT;
  flash_write_block(root_start, format_buf);
  memset(format_buf, 0, BLOCK_SIZE);
  for (uint32_t i = 1; i < ROOT_SECTORS; i++) {
    flash_write_block(root_start + i, format_buf);
  }

  createConfigFile();
}

uint8_t Storage::ram_disk[RAM_DISK_BLOCKS * BLOCK_SIZE];

int32_t Storage::msc_ram_read_cb(uint32_t lba, void *buffer, uint32_t bufsize) {
  memcpy(buffer, ram_disk + lba * BLOCK_SIZE, bufsize);
  return bufsize;
}

int32_t Storage::msc_ram_write_cb(uint32_t lba, uint8_t *buffer,
                                  uint32_t bufsize) {
  memcpy(ram_disk + lba * BLOCK_SIZE, buffer, bufsize);
  return bufsize;
}

void Storage::msc_ram_flush_cb(void) {}

void Storage::format_ram_disk() {
  memset(ram_disk, 0, sizeof(ram_disk));
  uint8_t *buf = ram_disk;

  // boot sector
  buf[0] = 0xEB;
  buf[1] = 0x3C;
  buf[2] = 0x90;
  buf[11] = 0x00;
  buf[12] = 0x02;            // bytes per sector
  buf[13] = 1;               // sectors per cluster
  buf[14] = 1;               // reserved sectors
  buf[16] = 2;               // num FATs
  buf[17] = 16;              // root entries
  buf[19] = RAM_DISK_BLOCKS; // total sectors
  buf[21] = 0xF8;            // media type
  buf[22] = 1;               // sectors per FAT
  buf[510] = 0x55;
  buf[511] = 0xAA;

  // FAT1
  uint8_t *fat1 = ram_disk + BLOCK_SIZE;
  fat1[0] = 0xF8;
  fat1[1] = 0xFF;
  fat1[2] = 0xFF;

  // FAT2
  uint8_t *fat2 = ram_disk + 2 * BLOCK_SIZE;
  fat2[0] = 0xF8;
  fat2[1] = 0xFF;
  fat2[2] = 0xFF;

  // volume label in root directory
  uint8_t *root = ram_disk + 3 * BLOCK_SIZE;
  memcpy(root, "LAIKA TEMP ", 11); // 11 chars, space padded
  root[11] = 0x08;                 // ATTR_VOLUME_ID
}

void Storage::begin() {
  delay(3000);
  Serial.println("Starting...");
  if (!is_formatted()) {
    Serial.println("Formatting as FAT16...");
    format_fat16();
    Serial.println("Done");
  } else {
    Serial.println("Already formatted");
  }

  format_ram_disk();

  // usb_msc.setID("RaspberryPi", "Pico Flash", "1.0");
  // usb_msc.setCapacity(BLOCK_COUNT, BLOCK_SIZE);
  // usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  // usb_msc.setUnitReady(true);
  usb_msc.setMaxLun(2);
  usb_msc.setID(0, "RaspberryPi", "Pico Flash", "1.0");
  usb_msc.setID(1, "RaspberryPi", "Pico RAM", "1.0");
  usb_msc.setCapacity(0, BLOCK_COUNT, BLOCK_SIZE);
  usb_msc.setCapacity(1, RAM_DISK_BLOCKS, BLOCK_SIZE);
  usb_msc.setReadWriteCallback(0, msc_read_cb, msc_write_cb, msc_flush_cb);
  usb_msc.setReadWriteCallback(1, msc_ram_read_cb, msc_ram_write_cb,
                               msc_ram_flush_cb);
  usb_msc.setUnitReady(0, true);
  usb_msc.setUnitReady(1, true);
  usb_msc.begin();

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

void Storage::task() {
  if (debug && millis() - last_write_ms > 1000) {
    debug = false;
    //   for (int i = 0; i < FLASH_SECTOR_SIZE; i++) {
    //     Serial.print((char)sector_buf[i]);
    //   }
    // }

    uint8_t data[100000];

    // "TEST.TXT" → pad to "TEST    TXT"
    Serial.println(debugLBA);
    debugLBA = 0;
    int32_t len = read_file("CONFIG  JSN", data, sizeof(data));
    if (len >= 0) {
      data[len] = '\0';
      Serial.println((char *)data);
    } else {
      Serial.println("File not found");
    }
    // if (TinyUSBDevice.mounted()) {
    //   TinyUSBDevice.detach();
    //   delay(10);
    //   TinyUSBDevice.attach();
    // }
    delay(100);
  }
}

int32_t Storage::msc_read_cb(uint32_t lba, void *buffer, uint32_t bufsize) {
  memcpy(buffer, (void *)(XIP_BASE + FS_OFFSET + lba * BLOCK_SIZE), bufsize);
  return bufsize;
}

int32_t Storage::msc_write_cb(uint32_t lba, uint8_t *buffer, uint32_t bufsize) {
  flash_write_block(lba, buffer);
  if (lba >= DATA_START) {
    last_write_ms = millis();
    debugLBA = lba;
    debug = true;
  }
  return bufsize;
}

void Storage::msc_flush_cb(void) {}
