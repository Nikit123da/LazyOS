// #include "file.h"
// #include "../drivers/VGA/VGA.h"
// #include "../memory/heap/kheap.h"
// #include "../memory/memory.h"
// #include "../status.h"
// filesystem *filesystems[LAZY_OS_MAX_FILESYSTEMS];
// file_descriptor *file_descriptors[LAZYOS_MAX_FILEDESCRIPTORS];
//
// static filesystem **fs_get_free_filesystem() {
//   int i = 0;
//
//   for (; i < LAZY_OS_MAX_FILESYSTEMS; i++) {
//     if (filesystems[i] == 0) {
//       return &filesystems[i];
//     }
//   }
//
//   return 0;
// }
//
// void fs_insert_filesystem(filesystem *filesystem) {
//   filesystem **fs;
//   // if(filesystem == 0){
//   //
//   // }
//   fs = fs_get_free_filesystem();
//   if (!fs) {
//     print("Problem inserting filesystem");
//     while (1) {
//     }
//   }
//
//   *fs = filesystem;
// }
//
// static void fs_static_load() { fs_insert_filesystem(fat12_init()); }
//
// void fs_laod() {
//   memset(filesystems, 0, sizeof(filesystem));
//   fs_static_load();
// }
//
// void fs_init() {
//   memset(file_descriptors, 0, sizeof(file_descriptors));
//   fs_laod();
// }
//
// static int file_new_descripter(file_descriptor **desc_out) {
//   int res = -NO_MEMORY;
//   for (int i = 0; i < LAZYOS_MAX_FILEDESCRIPTORS; i++) {
//     if (file_descriptors[i] == 0) {
//       file_descriptor *desc = kzalloc(sizeof(file_descriptor));
//       // descriptors start at 1
//       desc->index = i + 1;
//       file_descriptors[i] = desc;
//       *desc_out = desc;
//       res = 0;
//       break;
//     }
//   }
//
//   return res;
// }
//
// static file_descriptor *file_get_descripter(int fd) {
//   if (fd <= 0 || fd >= LAZYOS_MAX_FILEDESCRIPTORS) {
//     return 0;
//   }
//
//   int index = fd - 1;
//
//   return file_descriptors[index];
// }
