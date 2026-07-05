#include "hashset.h"

int main(void) {
  Set(int) set;
  set_init(set, 32);

  set_add(set, 42);
  set_add(set, 7);
  set_add(set, 42);

  assert(set_contains(set, 7));
  assert(!set_contains(set, 99));
  assert(set_len(set) == 2);
  assert(set_remove(set, 7));
  assert(!set_remove(set, 7));
  assert(set_len(set) == 1);

  size_t count = 0;
  set_for(set, value) {
    assert(value == 42);
    ++count;
  }
  assert(count == 1);
  set_free(set);

  Set(const char *) names;
  set_init_with(names, 2, gc_str_hash, gc_str_eq);
  set_add(names, "alice");
  set_add(names, "bob");
  set_add(names, "alice");
  set_add(names, "carol");

  assert(set_contains(names, "alice"));
  assert(set_contains(names, "bob"));
  assert(set_contains(names, "carol"));
  assert(!set_contains(names, "dave"));
  assert(set_len(names) == 3);
  set_free(names);
}
