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

  printf("All elements:\n");
  set_for(s, x) { printf("  %d\n", x); }

  set_free(s);
  return 0;
}
