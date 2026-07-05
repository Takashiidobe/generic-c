#include "tuple.h"

#include <assert.h>
#include <math.h>
#include <string.h>

int main(void) {
  __auto_type pair = make_tuple(7, 2.71828f);
  assert(pair._0 == 7);
  assert(fabsf(pair._1 - 2.71828f) < 1e-6f);
  assert(tuple_get(pair, 0) == 7);

  __auto_type mixed = make_tuple("hello", 123u);
  assert(strcmp(tuple_get(mixed, 0), "hello") == 0);
  assert(tuple_get(mixed, 1) == 123u);
}
