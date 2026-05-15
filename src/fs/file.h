#ifndef FILE_H
#define FILE_H

// #include "../disk/disk.h"
// #include "pparser.h"
// #define LAZYOS_MAX_FILEDESCRIPTORS 512
// #define LAZY_OS_MAX_FILESYSTEMS 3
//
// typedef unsigned int FILE_SEEK_MODE;
// enum { SEEK_SET, SEEK_CUR, SEEK_END };
//
// struct disk;
//
// typedef unsigned int FILE_MODE;
// enum { FILE_MODE_READ, FILE_MODE_WRITE, FILE_MODE_APPEND, FILE_MODE_INVAKID
// }; typedef void *(*FS_OPEN_FUNCTION)(disk *disk, struct path_part *path,
//                                   FILE_MODE mode);
//
// typedef int (*FS_RESOLVE_FUNCTION)(disk *disk);
//
// typedef struct filesystem {
//   // resolve = 0 if disk using it's file system
//   FS_RESOLVE_FUNCTION resolve;
//   FS_OPEN_FUNCTION open;
//
//   char name[20];
// } filesystem;
//
// typedef struct file_descriptor {
//   // descriptor index
//   int index;
//   filesystem *filesystem;
//
//   // private data for internal file descriptors
//   void *prvt;
//
//   // the disk that the file descriptor shopuld be used on
//   disk *disk;
// } file_descriptor;
//
// void fs_init();
// int fopen(const char *filename, const char *mode);
// void fs_insert_filesystem(filesystem *filesystem);
// filesystem *fs_resolve(disk *disk);
#endif // FILE_H
