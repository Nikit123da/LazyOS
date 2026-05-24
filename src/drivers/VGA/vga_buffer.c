#include "../../fs/fat/fat12.h"
#include "../../memory/heap/kheap.h"
#include "../../memory/memory.h"
#include "../../str/str.h"
#include "VGA.h"
#include "system_commands.h"
#include <stdint.h>

#define MAX_ARGS 2
mode mode_t = TERMINAL;
static uint8_t position = 0;
static uint8_t e_pos = 0;
static char editor_filename[10];

char *buff = NULL;
char editor_buff[4096];

void init_keyboard_buffer() { buff = kzalloc(4096); }

void inputIntoBuffer(char ch) {
  if (position >= 4096) {
    print("\nThe buffer is full\n");
    return;
  }
  buff[position] = ch;
  position += 1;
}

void clearBuffer() {
  memset(buff, 0, 4096);
  position = 0;
}

void remove_one_from_buffer() {
  if (position == 0)
    return;
  memset(buff + position - 1, 0, 1);
  position -= 1;
}

void inputIntoEditorBuffer(char ch) {
  if (position >= 4096) {
    print("\nThe buffer is full\n");
    return;
  }
  editor_buff[e_pos] = ch;
  e_pos += 1;
}

void clearEditorBuffer() {
  memset(editor_buff, 0, 4096);
  e_pos = 0;
}

void remove_one_from_editor_buffer() {
  if (e_pos == 0)
    return;
  memset(editor_buff + e_pos - 1, 0, 1);
  e_pos -= 1;
}

void bufferTestCommand() {
  char *argv[MAX_ARGS];
  int argc = 0;

  argv[argc++] = buff;
  for (int i = 0; buff[i] != '\0' && argc < MAX_ARGS; i++) {
    if (buff[i] == ' ') {
      buff[i] = '\0';
      argv[argc++] = &buff[i + 1];
    }
  }

  if (argv[0][0] == '\0') {
    return;
  }

  if (strcomp(argv[0], "test")) {
    test_com();
  } else if (strcomp(argv[0], "clear")) {
    terminal_clear();
  } else if (strcomp(argv[0], "get-time")) {
    get_time();
  } else if (strcomp(argv[0], "read")) {
    if (argc < 2) {
      print("usage: read <filename>\n");
    } else {
      read_file(argv[1]);
    }
  } else if (strcomp(argv[0], "write")) {
    if (argc < 2) {
      print("usage: write <filename>\n");
    } else {
      int i = 0;
      while (i < 10 && argv[1][i] != '\0') {
        editor_filename[i] = argv[1][i];
        i++;
      }
      editor_filename[i] = '\0';
      save_window_content();
      enter_editor_mode(argv[1]);
      mode_t = EDITOR;
    }
  } else if (strcomp(argv[0], "delete")) {
    if (argc < 2) {
      print("usage: delete <filename>\n");
    } else {
      delete_file(argv[1]);
    }
  } else if (strcomp(argv[0], "ls")) {
    list_files();
  }

  else {
    print("Command not recognized\n");
  }
}

void c_file() { create_file(editor_filename, "txt", editor_buff, e_pos, 0x20); }

void print_buff() { print(buff); }
