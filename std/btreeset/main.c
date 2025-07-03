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

  printf("Elements in order:\n");
  bset_for(s, x) { printf("  %d\n", x); }

  bset_free(s);
  return 0;
}
