#include "vecdeque.h"
#include "../test/assert.h"

int main(void) {
  Deque(int) deque = NULL;
  GC_ASSERT(dq_len(deque) == 0);
  GC_ASSERT(dq_cap(deque) == 0);

  dq_push_back(deque, 1);
  dq_push_back(deque, 2);
  dq_push_back(deque, 3);
  GC_ASSERT(dq_len(deque) == 3);
  GC_ASSERT(dq_cap(deque) >= 3);

  dq_push_front(deque, 0);
  dq_push_front(deque, -1);
  GC_ASSERT(dq_len(deque) == 5);

  const int expected[] = {-1, 0, 1, 2, 3};
  for (size_t i = 0; i < dq_len(deque); ++i)
    GC_ASSERT(dq_get(deque, i) == expected[i]);

  GC_ASSERT(dq_pop_front(deque) == -1);
  GC_ASSERT(dq_pop_back(deque) == 3);
  GC_ASSERT(dq_len(deque) == 3);

  const int remaining[] = {0, 1, 2};
  size_t index = 0;
  deque_for(item, deque) { GC_ASSERT(*item == remaining[index++]); }
  GC_ASSERT(index == 3);

  dq_free(deque);
  GC_ASSERT(deque == NULL);
  GC_ASSERT(dq_len(deque) == 0);
}
