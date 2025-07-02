#include "vecdeque.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  // Create an empty deque of int
  Deque(int) dq = NULL;

  // Push back 1, 2, 3
  dq_push_back(dq, 1);
  dq_push_back(dq, 2);
  dq_push_back(dq, 3);
  printf("After push_back: len=%zu, cap=%zu\n", dq_len(dq), dq_cap(dq));
  printf("Contents: ");
  for (size_t i = 0; i < dq_len(dq); ++i) {
    printf("%d ", dq_get(dq, i));
  }
  printf("\n");

  // Push front 0, -1
  dq_push_front(dq, 0);
  dq_push_front(dq, -1);
  printf("After push_front: len=%zu, cap=%zu\n", dq_len(dq), dq_cap(dq));
  printf("Contents: ");
  for (size_t i = 0; i < dq_len(dq); ++i) {
    printf("%d ", dq_get(dq, i));
  }
  printf("\n");

  // Pop front and back
  int front = dq_pop_front(dq);
  int back = dq_pop_back(dq);
  printf("Popped front=%d, back=%d\n", front, back);
  printf("After pops: len=%zu\n", dq_len(dq));
  printf("Contents via deque_for: ");
  int *it;
  deque_for(it, dq) { printf("%d ", *it); }
  printf("\n");

  // Free deque
  dq_free(dq);
  printf("Deque freed, len=%zu\n", dq_len(dq));
  return 0;
}
