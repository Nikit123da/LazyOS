#include "disk.h"
#include "../IO/io.h"
// NOTE: maybe add an inode.
int disk_read_secotr(int lba, int total, void *buff) {
  outb(0x1f6, (lba >> 24) | 0xE0); // asks for the primary disk with 0xE0
  outb(0x1f2, total);              // how many sectors to read
  outb(0x1f3, (unsigned char)(lba & 0xff));
  outb(0x1f4, (unsigned char)(lba >> 8));
  outb(0x1f5, (unsigned char)(lba >> 16));
  outb(0x1f7, 0x20);

  unsigned short *ptr = (unsigned short *)buff;

  for (int i = 0; i < total; i++) {
    // wait fot the buffer to be ready
    while (!(insb(0x1f7) & 0x08)) {
    } // check for the flags that tell if the disk is ready

    // copy from hard disk to memory
    for (i = 0; i < 256; i++) {
      // reading fro, the primary hard disk
      *ptr = insw(0x1f0); // read to buffer
      ptr++;
    }
  }
  return 0;
}
