#ifndef FAT12_H
#define FAT12_H
#include "../../clock/clock.h"
#include <stdint.h>
#define TOTAL_FAT12_ENTREIS 2648 // 233 - 2880
#define END_OF_FILE 0xFFF
#define FREE_ENTRY 0x00
#define FILE 0x20
#define DIRECTORY 0X10
#define STARTING_DATA_SECTOR 233
#define FAT1_SECTOR 202
#define START_ROOT_DIR 219
#define SECTOR_SIZE 512

typedef struct {

  char name[8];           // 1 byte
  char ext[3];            // 1 byte
  uint16_t first_cluster; // 2 bytes
  uint8_t attribute;      // 0x20 - file 0x10 - dir
  rtc_time time_stamp;    // 6 bytes
  uint32_t size;          // 4 bytes

} file_dir; // 15 bytes

void FAT12_table_init();
file_dir *create_file(const char *name_t, const char *ext_t, void *data,
                      uint32_t size, uint8_t attribute_t);
void read_file(char *name);
void delete_file(char *name);
void list_files();
#endif // !FAT12_H
