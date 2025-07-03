#include "hashset.h"
#include <stdio.h>

int main(void) {
  Set(int) s;
  set_init(s, 32);

  set_add(s, 42);
  set_add(s, 7);
  set_add(s, 42); // duplicate, no effect

  printf("Contains 7?  %d\n", set_contains(s, 7));
  printf("Contains 99? %d\n", set_contains(s, 99));

  printf("len = %zu\n", set_len(s));
  printf("remove 7 -> %d\n", set_remove(s, 7));
  printf("remove 7 again -> %d\n", set_remove(s, 7));
  printf("len = %zu\n", set_len(s));

  printf("All elements:\n");
  set_for(s, x) { printf("  %d\n", x); }

  set_free(s);

  // string elements compared by content, plus growth past initial buckets
  Set(const char *) names;
  set_init_with(names, 2, gc_str_hash, gc_str_eq);
  set_add(names, "alice");
  set_add(names, "bob");
  set_add(names, "alice"); // duplicate by content, no effect
  set_add(names, "carol");
  printf("names: contains bob? %d, len = %zu\n", set_contains(names, "bob"),
         set_len(names));
  set_free(names);
  return 0;
}
