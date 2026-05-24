#include "queue.h"
#include "../memory/heap/kheap.h"

static void swap(priority_queue *q, int a, int b) {
  process *tmp = q->data[a];
  q->data[a] = q->data[b];
  q->data[b] = tmp;
}

static void bubble_up(priority_queue *q, int i) {
  while (i > 0) {
    int parent = (i - 1) / 2;
    if (q->data[i]->priority > q->data[parent]->priority) {
      swap(q, i, parent);
      i = parent;
    } else {
      break;
    }
  }
}

static void bubble_down(priority_queue *q, int i) {
  while (1) {
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int largest = i;

    if (left < q->size && q->data[left]->priority > q->data[largest]->priority)
      largest = left;
    if (right < q->size &&
        q->data[right]->priority > q->data[largest]->priority)
      largest = right;

    if (largest == i)
      break;

    swap(q, i, largest);
    i = largest;
  }
}

void pq_init(priority_queue *q) { q->size = 0; }

void pq_push(priority_queue *q, process *proc) {
  if (q->size >= MAX_PROCESSES)
    return;
  q->data[q->size] = proc;
  bubble_up(q, q->size);
  q->size++;
}

process *pq_pop(priority_queue *q) {
  if (q->size == 0)
    return 0;
  process *top = q->data[0];
  q->size--;
  q->data[0] = q->data[q->size];
  bubble_down(q, 0);
  return top;
}

process *pq_peek(priority_queue *q) { return q->size ? q->data[0] : 0; }

int pq_is_empty(priority_queue *q) { return q->size == 0; }
