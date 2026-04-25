#include "streamer.h"
#include "../config.h"
#include "../memory/heap/kheap.h"
#include "disk.h"

disk_stream *disk_streamer_new(int disk_id) {
  disk *disk = disk_get(disk_id);
  if (!disk) {
    return 0;
  }

  disk_stream *streamer = kzalloc(sizeof(disk_stream));
  streamer->pos = 0;
  streamer->disk = disk;
  return streamer;
}

int disk_streamer_seek(disk_stream *stream, int pos) {
  stream->pos = pos;
  return 0;
}

int disk_streamer_read(disk_stream *stream, void *out, int total,
                       int total_recursions) { // NOTE: total_recursions has to
                                               // be initialized to 0;
  int sector = stream->pos / LAZYOS_SECTOR_SIZE;
  int offset = stream->pos % LAZYOS_SECTOR_SIZE;
  char buff[LAZYOS_SECTOR_SIZE];

  int res = disk_read_block(stream->disk, sector, 1, buff);
  if (res < 0) {
    return res;
  }

  int total_to_read = total > LAZYOS_SECTOR_SIZE ? LAZYOS_SECTOR_SIZE : total;
  for (int i = 0; i < total_to_read; i++) {
    *(char *)out++ = buff[offset + i];
  }

  if (total_recursions > STACK_OVERFLOW_FLAG) {
    return -1;
  }

  stream->pos += total_to_read;
  if (total > LAZYOS_SECTOR_SIZE) {
    res = disk_streamer_read(stream, out, total - LAZYOS_SECTOR_SIZE,
                             total_recursions++);
  }

  return res;
}

void disk_streamer_close(disk_stream *stream) { kfree(stream); }
