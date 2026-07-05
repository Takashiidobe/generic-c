#include "btreeset.h"

#include <string.h>

int main(void) {
  BSet(int) set;
  bset_init(set, 4);

  bset_add(set, 10);
  bset_add(set, 5);
  bset_add(set, 7);
  bset_add(set, 5);

  assert(bset_contains(set, 7));
  assert(!bset_contains(set, 3));
  assert(ss_len(set) == 3);
  assert(bset_remove(set, 7));
  assert(!bset_remove(set, 7));
  assert(ss_len(set) == 2);

  const int expected[] = {5, 10};
  size_t index = 0;
  bset_for(set, value) { assert(value == expected[index++]); }
  assert(index == 2);
  bset_free(set);

  BSet(const char *) names;
  bset_init_with(names, 1, gc_str_cmp);
  bset_add(names, "charlie");
  bset_add(names, "alice");
  bset_add(names, "bob");
  bset_add(names, "alice");

  const char *ordered[] = {"alice", "bob", "charlie"};
  index = 0;
  bset_for(names, value) {
    assert(strcmp(value, ordered[index]) == 0);
    ++index;
  }
  assert(index == 3);
  bset_free(names);
}
