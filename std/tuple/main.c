#include <stdio.h>

// pretend we have the above two macros in "tuple.h"
#include "tuple.h"

// but most of the time you can *infer* the types automatically:
#define auto __auto_type // GCC/Clang extension

int main(void) {
  auto p2 = make_tuple(7, 2.71828f);
  // p2 is now a struct { int first; float second; }
  printf("p2 = (%d, %.5f)\n", p2.first, p2.second);

  auto p3 = make_tuple("hello", 123u);
  // p3 is struct { const char *first; unsigned second; }
  printf("p3 = (%s, %u)\n", p3.first, p3.second);

  return 0;
}
