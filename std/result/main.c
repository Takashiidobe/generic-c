#include "result.h"
#include <stdio.h>
#include <stdlib.h> // for strtol

DEFINE_RESULT(ParseIntResult, int, const char *);

// crosses a function boundary: the value returned here is assigned to a
// ParseIntResult in main, which only works because both share the named type.
static ParseIntResult parse_int(const char *s) {
  char *end;
  long val = strtol(s, &end, 10);
  if (*end != '\0')
    return RESULT_ERR(ParseIntResult, "invalid integer");
  return RESULT_OK(ParseIntResult, (int)val);
}

int main(void) {
  ParseIntResult ok = parse_int("123");
  if (result_is_ok(ok))
    printf("Parsed: %d\n", unwrap(ok));

  ParseIntResult bad = parse_int("12x");
  if (result_is_err(bad))
    printf("Error: %s\n", unwrap_err(bad));
  return 0;
}
