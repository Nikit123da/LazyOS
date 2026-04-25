#include "pparser.h"
#include "../kernel.h"
#include "../memory/heap/kheap.h"
#include "../memory/memory.h"
#include "../status.h"
#include "../str/str.h"
// insure the path is a valid format
static int pathparser_path_valid_format(const char *filename) {
  int len = strnlen(filename, LAZYOS_MAX_PATH);
  return (len >= 3 && is_digit(filename[0]) &&
          memcmp((void *)&filename[1], ":/", 2) == 0);
}

static int pathparser_get_drive_by_path(const char **path) {
  if (!pathparser_path_valid_format(*path)) {
    return -EBADPATH;
  }

  int drive_num = to_numberic_digit(*path[0]);

  *path += 3; // jump the 0:/ and only get the path itself, instead of
              // 0:/test.txt just test.txt
  return drive_num;
}

static pathroot *pathparser_create_root(int drive_num) {
  pathroot *path_r = kzalloc(sizeof(pathroot));
  path_r->drive_no = drive_num;
  path_r->first = 0;
  return path_r;
}

static const char *pathparser_get_path_part(const char **path) {
  char *result_path_part = kzalloc(LAZYOS_MAX_PATH);
  int i = 0;
  while (**path != '/' && **path != 0x00) {
    result_path_part[i] = **path;
    *path += 1;
    i++;
  }

  if (**path == '/') { // skip /
    *path += 1;
  }

  if (i == 0) {
    kfree(result_path_part);
    result_path_part = 0;
  }

  return result_path_part;
}

path_part *pathparser_parse_path_part(path_part *last_part, const char **path) {
  const char *path_part_string = pathparser_get_path_part(path);
  if (!path_part_string) {
    return 0;
  }

  path_part *part = kzalloc(sizeof(path_part));
  part->part = path_part_string;
  part->next = 0x00;

  if (last_part) {
    last_part->next = part;
  }

  return part;
}

void pathparser_free(pathroot *root) {
  path_part *part = root->first;
  while (part) {
    path_part *next_part = part->next;
    kfree((void *)part->part);
    kfree(part);
    part = next_part;
  }

  kfree(root);
}

pathroot *pathparser_parse(const char *path,
                           const char *current_directory_path) {
  int res = 0;
  const char *tmp_path = path;
  pathroot *path_root = 0;

  if (strlen(path) > LAZYOS_MAX_PATH) {
    return path_root;
  }

  res = pathparser_get_drive_by_path(&tmp_path);
  if (res < 0) {
    return path_root;
  }

  path_root = pathparser_create_root(res);
  if (!path_root) {
    return path_root;
  }

  path_part *first_path = pathparser_parse_path_part(NULL, &tmp_path);
  if (!first_path) {
    return path_root;
  }

  path_root->first = first_path;
  path_part *part = pathparser_parse_path_part(first_path, &tmp_path);
  while (part) {
    part = pathparser_parse_path_part(part, &tmp_path);
  }

  return path_root;
}
