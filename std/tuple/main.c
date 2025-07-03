#include <stdio.h>

#include "tuple.h"

int main(void) {
  // __auto_type (not C23 auto): gcc forbids defining the anonymous tuple
  // struct type inside a C23 auto initializer. fields are _0, _1, ...
  __auto_type p2 = make_tuple(7, 2.71828f);
  printf("p2 = (%d, %.5f)\n", p2._0, p2._1);

  __auto_type p3 = make_tuple("hello", 123u);
  printf("p3 = (%s, %u)\n", tuple_get(p3, 0), tuple_get(p3, 1));

  return 0;
}
