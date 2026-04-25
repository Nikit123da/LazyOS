#ifndef PATHPARSER_H
#define PATHPARSER_H

typedef struct path_part {
  const char *part;
  struct path_part *next;
} path_part;

typedef struct {
  int drive_no;
  path_part *first;
} pathroot;

pathroot *pathparser_parse(const char *path,
                           const char *current_directory_path);
void pathparser_free(pathroot *root);
// 0:test/test.txt
// pathroot/path_part_1/pathpart_2.txt
#endif // !PATHPARSER_H
