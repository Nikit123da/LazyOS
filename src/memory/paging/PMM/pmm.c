#include "pmm.h"
#include "../../../config.h"
#include "../../heap/heap.h"
#include "../../heap/kheap.h"
#include "../paging.h"
#include "./../../../status.h"
#include <stddef.h>
#include <stdint.h>

// TODO: Frames calculation, then somehow include these frames into the giving
// info RAM;

uint8_t *bitmap;
uint8_t *curr_free_space;

int calc_frames(uint64_t size) {
  if (size >= 0x100000000 - 1)
    return 0x100000000 - 0x1; // above 4GB safty
  return (size + 4095) / 4096;
}

void devide_blocks_entries(uint32_t frames, uint32_t *full, uint8_t *not_full) {
  *full = frames / 8;
  *not_full = frames % 8;
}

// uint8_t checker_size(uint32_t frames) {
//   uint8_t checker;
//   for (int i = 0; i < frames; i++) {
//     checker = checker << 1 | 0b00000001;
//   }
//   return checker;
// }

// void marked_as_taken(uint32_t full_entry, uint32_t not_full_entry,
//                      uint32_t start) {
//   int i = 0;
//   for (i = 0x100000; i < full_entry; i++) {
//     bitmap[i] = WHOLE_ENTRY_TAKN;
//   }
//
//   uint8_t checker = checker_size(not_full_entry);
//   bitmap[i] = bitmap[i] | checker;
// }

void fill_from_to(uint32_t base, uint32_t len) {
  /*tail*/
  if (len <= 0) {
    return;
  }
  uint32_t len_in_frames = calc_frames(len);
  uint64_t start_frame =
      calc_frames(base); // acts as the offset from the begining
                         // frame, meaning the first frame here.

  uint32_t block = start_frame / 8;
  uint8_t frame_in_block = start_frame % 8;

  uint8_t marker = 0b00000001;

  marker = marker << frame_in_block;

  bitmap[block] |= marker;
  len_in_frames--;

  if (len_in_frames <= 0) {
    return;
  }

  uint32_t full_block = 0;
  uint8_t not_full_block = 0;

  devide_blocks_entries(len_in_frames, &full_block, &not_full_block);
  uint8_t filler = 0b10000000;
  for (int i = 0; i < 6 - frame_in_block; i++) {

    if (full_block == 0 && not_full_block == 0) {
      return; // if no more frames needed
    }

    if (not_full_block == 0) {
      full_block--;
      not_full_block = 7;
    }
    filler = filler >> 1;
    filler |= 0b10000000;
    not_full_block--;
  }

  bitmap[block] |=
      filler; // filled the block, can go onto filling the other blocks

  /*body*/
  while (full_block > 0) {
    bitmap[++block] = WHOLE_ENTRY_TAKN;
    full_block--;
  }
  // full_block = 0, for sure;

  /*head*/
  marker = 0b00000000;
  while (not_full_block > 0) {
    marker = marker << 1;
    marker |= 0b00000001;
    not_full_block--;
  }

  bitmap[++block] |= marker;
}

void manual_prefill() {
  uint64_t heap_start = HEAP_START_ADDRESS;
  uint64_t heap_end = HEAP_START_ADDRESS + LAZYOS_HEAP_SIZE_BYTES;

  fill_from_to(0x100000, 0x1FA000);
  fill_from_to(heap_start, heap_end);
}

void init_bitmap_PMM() {
  int i;
  bitmap = kzalloc(sizeof(uint8_t) * PMM_ENTRIES);
  for (i = 0; i < PMM_ENTRIES; i++) {
    bitmap[i] = FREE_PMM_ENTRY;
  }

  manual_prefill();
  curr_free_space = bitmap;
}

int get_free_physical_address() {
  uint8_t i = 0;
  while (curr_free_space[i] == 255) {
    i++;
  } // Now at a block that can give a frame

  curr_free_space += i;

  // find what bit is responsible for the empty frame. looking for a 0
  uint8_t marker = 0b00000001;
  int j;
  for (j = 0; j < 8; j++) {
    if (!(curr_free_space[i] ^ marker)) { // if 0
      break;
    }
    marker = (marker << 1) | 0b00000001;
  }

  int byte_offset = curr_free_space - bitmap;
  return (byte_offset * 8 + j) * 4096;
}
