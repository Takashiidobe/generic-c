#include "btreeset.h"
#include <stdio.h>

int main(void) {
  BSet(int) s;
  bset_init(s, 4);

  bset_add(s, 10);
  bset_add(s, 5);
  bset_add(s, 7);
  bset_add(s, 5); // duplicate

  printf("Contains 7?  %d\n", bset_contains(s, 7));
  printf("Contains 3?  %d\n", bset_contains(s, 3));

  printf("len = %zu\n", ss_len(s));
  printf("remove 7 -> %d\n", bset_remove(s, 7));
  printf("remove 7 again -> %d\n", bset_remove(s, 7));
  printf("len = %zu\n", ss_len(s));

  printf("Elements in order:\n");
  bset_for(s, x) { printf("  %d\n", x); }

  bset_free(s);

  // string elements sorted by content via gc_str_cmp
  BSet(const char *) names;
  bset_init_with(names, 1, gc_str_cmp);
  bset_add(names, "charlie");
  bset_add(names, "alice");
  bset_add(names, "bob");
  bset_add(names, "alice"); // duplicate by content
  printf("names in order:\n");
  bset_for(names, x) { printf("  %s\n", x); }
  bset_free(names);
  return 0;
}
