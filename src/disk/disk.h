#ifndef DISK_H
#define DISK_H

typedef unsigned int LAZYOS_DISK_TYPE;

// real physical hard disk
#define LAZYOS_DISK_TYPE_REAL 0;

typedef struct {
  LAZYOS_DISK_TYPE type;
  int sector_size;
} disk;

void disk_search_and_init();
disk *disk_get(int index);
int disk_read_block(disk *idisk, unsigned int lba, int total, void *buff);
#endif // !DEBUG
