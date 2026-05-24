#include <stdint.h>
#include <stdio.h>

#define MAX_PROCESSES 256
#define KERNEL_PROCESS_STACK_SIZE 4096

typedef enum {
  PROCESS_UNUSED,
  PROCESS_READY,
  PROCESS_WAITING,
  PROCESS_RUNNING,
  PROCESS_TERMINATED
} process_state;

typedef struct {
  uint32_t sp;
  void *virt_addr;
  process_state state;
  uint8_t priority;
  uint8_t PID;
  uint8_t proc_stack[KERNEL_PROCESS_STACK_SIZE];
  float burst_time;
} process;

// ---- priority queue (max-heap) ----

typedef struct {
  process *data[MAX_PROCESSES];
  uint16_t size;
} priority_queue;

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

// ---- tests ----

static process make_proc(uint8_t pid, uint8_t priority) {
  process p;
  // memset(&p, 0, sizeof(p));
  p.PID = pid;
  p.priority = priority;
  p.state = PROCESS_READY;
  return p;
}

int main() {
  priority_queue q;
  pq_init(&q);

  process p1 = make_proc(1, 10);
  process p2 = make_proc(2, 50);
  process p6 = make_proc(4, 100);
  process p3 = make_proc(3, 30);
  process p4 = make_proc(4, 70);
  process p5 = make_proc(5, 20);

  pq_push(&q, &p1);
  pq_push(&q, &p2);
  pq_push(&q, &p3);
  pq_push(&q, &p4);
  pq_push(&q, &p5);
  pq_push(&q, &p6);

  printf("Peek (should be PID=4 priority=70): PID=%d priority=%d\n",
         pq_peek(&q)->PID, pq_peek(&q)->priority);

  printf("\nPopping in priority order (expected 70 50 30 20 10):\n");
  while (!pq_is_empty(&q)) {
    process *p = pq_pop(&q);
    printf("  PID=%d priority=%d\n", p->PID, p->priority);
  }

  printf("\nPop from empty (should be NULL): %s\n",
         pq_pop(&q) == 0 ? "NULL" : "NOT NULL");

  return 0;
}
