#include "tuple.h"
#include "../test/assert.h"

#include <math.h>
#include <string.h>

int main(void) {
  __auto_type pair = make_tuple(7, 2.71828f);
  GC_ASSERT(pair._0 == 7);
  GC_ASSERT(fabsf(pair._1 - 2.71828f) < 1e-6f);
  GC_ASSERT(tuple_get(pair, 0) == 7);

  __auto_type mixed = make_tuple("hello", 123u);
  GC_ASSERT(strcmp(tuple_get(mixed, 0), "hello") == 0);
  GC_ASSERT(tuple_get(mixed, 1) == 123u);
}
