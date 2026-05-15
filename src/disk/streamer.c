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

int disk_streamer_read(disk_stream *stream, void *out, int total) {
  char buff[LAZYOS_SECTOR_SIZE];
  char *dst = (char *)out;
  int remaining = total;

  while (remaining > 0) {
    int sector = stream->pos / LAZYOS_SECTOR_SIZE;
    int offset = stream->pos % LAZYOS_SECTOR_SIZE;

    int res = disk_read_block(stream->disk, sector, 1, buff);
    if (res < 0)
      return res;

    int to_read = remaining > (LAZYOS_SECTOR_SIZE - offset)
                      ? (LAZYOS_SECTOR_SIZE - offset)
                      : remaining;

    for (int i = 0; i < to_read; i++)
      dst[i] = buff[offset + i];

    dst += to_read;
    stream->pos += to_read;
    remaining -= to_read;
  }

  return 0;
}

int disk_streamer_write(disk_stream *stream, void *in, int total) {
  char buff[LAZYOS_SECTOR_SIZE];
  char *src = (char *)in;
  int remaining = total;

  while (remaining > 0) {
    int sector = stream->pos / LAZYOS_SECTOR_SIZE;
    int offset = stream->pos % LAZYOS_SECTOR_SIZE;

    int res = disk_read_block(stream->disk, sector, 1, buff);
    if (res < 0)
      return res;

    int to_write = remaining > (LAZYOS_SECTOR_SIZE - offset)
                       ? (LAZYOS_SECTOR_SIZE - offset)
                       : remaining;

    for (int i = 0; i < to_write; i++)
      buff[offset + i] = src[i];

    res = disk_write_block(stream->disk, sector, 1, buff);
    if (res < 0)
      return res;

    src += to_write;
    stream->pos += to_write;
    remaining -= to_write;
  }

  return 0;
}

void disk_streamer_close(disk_stream *stream) { kfree(stream); }
