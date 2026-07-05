#include "hashset.h"
#include "../test/assert.h"

int main(void) {
  Set(int) set;
  set_init(set, 32);

  set_add(set, 42);
  set_add(set, 7);
  set_add(set, 42);

  GC_ASSERT(set_contains(set, 7));
  GC_ASSERT(!set_contains(set, 99));
  GC_ASSERT(set_len(set) == 2);
  GC_ASSERT(set_remove(set, 7));
  GC_ASSERT(!set_remove(set, 7));
  GC_ASSERT(set_len(set) == 1);

  size_t count = 0;
  set_for(set, value) {
    GC_ASSERT(value == 42);
    ++count;
  }
  GC_ASSERT(count == 1);
  set_free(set);

  Set(const char *) names;
  set_init_with(names, 2, gc_str_hash, gc_str_eq);
  set_add(names, "alice");
  set_add(names, "bob");
  set_add(names, "alice");
  set_add(names, "carol");

  GC_ASSERT(set_contains(names, "alice"));
  GC_ASSERT(set_contains(names, "bob"));
  GC_ASSERT(set_contains(names, "carol"));
  GC_ASSERT(!set_contains(names, "dave"));
  GC_ASSERT(set_len(names) == 3);
  set_free(names);
}
