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

  usb_msc.setID("RaspberryPi", "Pico Flash", "1.0");
  usb_msc.setCapacity(BLOCK_COUNT, BLOCK_SIZE);
  usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
  usb_msc.setUnitReady(true);
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
    int32_t len = read_file("TEST    TXT", data, sizeof(data));
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

  // debug = true;
  // for (int i = 0; i < bufsize; i++) {
  //   Serial.println(buffer[i]);
  //   delay(10);
  // }
  return bufsize;
}

void Storage::msc_flush_cb(void) {}
