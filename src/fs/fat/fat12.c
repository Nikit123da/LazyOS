#include "fat12.h"
#include "../../clock/clock.h"
#include "../../disk/disk.h"
#include "../../disk/streamer.h"
#include "../../drivers/VGA/VGA.h"
#include "../../memory/heap/kheap.h"
#include "../../status.h"
#include "../../str/str.h"
#include <stdint.h>
file_dir root_dir[224] = {0x00};
uint16_t *fat12_table;
int delete_buff[512] = {0x00}; // NOTE: use in free_file to delete the info of
                               // the file that want to be removed

void read_FAT12_table() {
  disk_stream *stream_out = disk_streamer_new(0);
  disk_streamer_seek(stream_out, FAT1_SECTOR * 512);
  disk_streamer_read(stream_out, fat12_table, TOTAL_FAT12_ENTREIS);
  disk_streamer_close(stream_out);
}

void FAT12_table_init() {
  fat12_table = (uint16_t *)kzalloc(sizeof(uint16_t) * TOTAL_FAT12_ENTREIS);
  read_FAT12_table();
}

int find_next_free(uint32_t from) {
  for (int i = from; i < TOTAL_FAT12_ENTREIS; i++) {
    if (fat12_table[i] == FREE_ENTRY)
      return i;
  }
  return -1;
}

int fat12_allocate(uint32_t size) {
  uint32_t sectors_needed = (size + 511) / 512;

  uint32_t first = find_next_free(0);
  if (first == -1)
    return -NO_MEMORY;

  uint32_t current = first;
  for (uint32_t i = 1; i < sectors_needed; i++) {
    uint32_t next = find_next_free(current + 1);
    if (next == -1)
      return -NO_MEMORY;

    fat12_table[current] = next;
    current = next;
  }
  fat12_table[current] = END_OF_FILE;

  return STARTING_DATA_SECTOR + first;
}

void free_file(uint32_t file_sector) { // file_sector = begining of a file
  while (fat12_table[file_sector] != END_OF_FILE) {
    file_sector = fat12_table[file_sector];
    fat12_table[file_sector] = FREE_ENTRY;
  }

  fat12_table[file_sector] = FREE_ENTRY;
}

void write_into_memory(file_dir *file, void *data) {
  disk_stream *stream_out = disk_streamer_new(0);

  // FILE DATA
  disk_streamer_seek(stream_out, file->first_cluster * 512); // works
  disk_streamer_write(stream_out, data, file->size);

  // FAT12 TABLE ITSELF
  disk_streamer_seek(stream_out,
                     FAT1_SECTOR *
                         512); // doesnt really work, maybe fat12_table empty
  disk_streamer_write(stream_out, fat12_table,
                      sizeof(uint16_t) * TOTAL_FAT12_ENTREIS);

  // FILE METADATA / name, time of creation, attribute etc...
  disk_streamer_seek(stream_out, START_ROOT_DIR * 512); // works
  disk_streamer_write(stream_out, root_dir, sizeof(root_dir));

  disk_streamer_close(stream_out);
}

file_dir *create_file(const char *name_t, const char *ext_t, void *data,
                      uint32_t size, uint8_t attribute_t) {
  file_dir new_file;

  rtc_time time_stamp = read_date();

  for (int i = 0; i < 8; i++)
    new_file.name[i] = (name_t[i] && i < strnlen(name_t, 8)) ? name_t[i] : ' ';
  for (int i = 0; i < 3; i++)
    new_file.ext[i] = (ext_t[i] && i < strnlen(ext_t, 3)) ? ext_t[i] : ' ';

  new_file.size = size;
  new_file.time_stamp = time_stamp;
  new_file.attribute = attribute_t;
  new_file.first_cluster = fat12_allocate(size);

  int i = 0;
  while (i < 224 && root_dir[i].name[0] != 0)
    i++;

  if (i == 224)
    return 0; // root dir full

  root_dir[i] = new_file;
  write_into_memory(&root_dir[i], data);
  return &root_dir[i];
}

int comp_file_name(char *ptr1, char *ptr2) {
  int len = strnlen(ptr2, 8);
  for (int i = 0; i < len; i++) {
    if (ptr1[i] != ptr2[i])
      return 0;
  }
  return 1;
}
void read_file(char *name) {
  uint8_t file_found = 0;
  uint32_t data_size;
  uint16_t next_cluser;
  uint16_t read;
  char *buff;
  disk_stream *stream_out = disk_streamer_new(0);

  disk_streamer_seek(stream_out, START_ROOT_DIR * 512);
  disk_streamer_read(stream_out, root_dir, sizeof(root_dir));

  for (int i = 0; i < 224; i++) {
    if (comp_file_name(root_dir[i].name, name)) {
      file_found = 1;
      data_size = root_dir[i].size;
      next_cluser = root_dir[i].first_cluster;
      break;
    }
  }

  if (file_found) {
    read = (data_size > SECTOR_SIZE) ? SECTOR_SIZE : data_size;
    buff = kzalloc(data_size);
    char *write_ptr = buff;
    while (fat12_table[next_cluser - STARTING_DATA_SECTOR] != END_OF_FILE) {
      read = (data_size > SECTOR_SIZE) ? SECTOR_SIZE : data_size;
      disk_streamer_seek(stream_out, next_cluser * 512);
      disk_streamer_read(stream_out, write_ptr, read);
      next_cluser = fat12_table[next_cluser - STARTING_DATA_SECTOR] +
                    STARTING_DATA_SECTOR;
      data_size -= read;
      write_ptr += read;
    }
    read = data_size;
    disk_streamer_seek(stream_out, next_cluser * 512);
    disk_streamer_read(stream_out, write_ptr, read);
    disk_streamer_close(stream_out);

    print(buff);
    print("\n");
    kfree(buff);
  }

  else {
    // ERROR NO FILE FOUND
    disk_streamer_close(stream_out);
    print("NO FILE FOUND\n");
  }
}
