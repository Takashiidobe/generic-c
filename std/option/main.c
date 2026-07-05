#include "option.h"

#include <math.h>

DEFINE_OPTION(int)
DEFINE_OPTION(double)

int main(void) {
  Option(int) some = OPTION_SOME(int, 42);
  Option(int) none = OPTION_NONE(int);

  assert(option_is_some(some));
  assert(!option_is_none(some));
  assert(unwrap_some(some) == 42);
  assert(option_is_none(none));
  assert(!option_is_some(none));
  assert(unwrap_or(none, 7) == 7);
  assert(unwrap_or(some, 7) == 42);

  Option(double) value = OPTION_SOME(double, 3.14);
  assert(option_is_some(value));
  assert(fabs(unwrap_some(value) - 3.14) < 1e-12);
}
