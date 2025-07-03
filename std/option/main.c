#include "option.h"
#include <stdio.h>

// 1) for each T you need, DEFINE_OPTION(T):
DEFINE_OPTION(int)
DEFINE_OPTION(double)

int main(void) {
  // 2) now you can write Option(int) and OPTION_SOME/OPTION_NONE:
  Option(int) a = OPTION_SOME(int, 42);
  Option(int) b = OPTION_NONE(int);

  if (option_is_some(a))
    printf("a = %d\n", unwrap_some(a));

  printf("b or 7 => %d\n", unwrap_or(b, 7));

  // and you have a separate Option(double>):
  Option(double) x = OPTION_SOME(double, 3.14);
  printf("x = %.2f\n", unwrap_some(x));

  return 0;
}
