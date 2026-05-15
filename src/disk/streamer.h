#ifndef STREAMER_H
#define STREAMER_H

#include "disk.h"

typedef struct {
  int pos;
  disk *disk;
} disk_stream;

disk_stream *disk_streamer_new(int disk_id);
int disk_streamer_seek(disk_stream *stream, int pos);
int disk_streamer_read(disk_stream *stream, void *out, int total);
void disk_streamer_close(disk_stream *stream);
int disk_streamer_write(disk_stream *stream, void *in, int total);

#endif // !STREAMER_H
