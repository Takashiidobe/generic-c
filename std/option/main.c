#include "option.h"
#include "../test/assert.h"

#include <math.h>

DEFINE_OPTION(int)
DEFINE_OPTION(double)

int main(void) {
  Option(int) some = OPTION_SOME(int, 42);
  Option(int) none = OPTION_NONE(int);

  GC_ASSERT(option_is_some(some));
  GC_ASSERT(!option_is_none(some));
  GC_ASSERT(unwrap_some(some) == 42);
  GC_ASSERT(option_is_none(none));
  GC_ASSERT(!option_is_some(none));
  GC_ASSERT(unwrap_or(none, 7) == 7);
  GC_ASSERT(unwrap_or(some, 7) == 42);

  Option(double) value = OPTION_SOME(double, 3.14);
  GC_ASSERT(option_is_some(value));
  GC_ASSERT(fabs(unwrap_some(value) - 3.14) < 1e-12);
}
