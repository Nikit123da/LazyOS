#include "queue.h"
#include "../memory/heap/kheap.h"

void queue_init(process_queue *q) {
  q->head = NULL;
  q->tail = NULL;
  q->size = 0;
}

void queue_push(process_queue *q, process *proc) {
  queue_node *node = (queue_node *)kzalloc(sizeof(queue_node));
  node->proc = proc;
  node->next = NULL;

  if (q->tail) {
    q->tail->next = node; // link to end
  } else {
    q->head = node; // first node, also the head
  }

  q->tail = node;
  q->size++;
}

process *queue_pop(process_queue *q) {
  if (!q->head)
    return NULL;

  queue_node *node = q->head;
  process *proc = node->proc;

  q->head = node->next;

  if (!q->head) {
    q->tail = NULL; // queue is now empty
  }

  kfree(node);
  q->size--;
  return proc;
}

process *queue_peek(process_queue *q) {
  if (!q->head)
    return NULL;
  return q->head->proc;
}

uint8_t queue_is_empty(process_queue *q) { return q->size == 0; }
