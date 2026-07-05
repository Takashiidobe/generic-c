#include "result.h"

#include <stdlib.h>
#include <string.h>

DEFINE_RESULT(ParseIntResult, int, const char *);

static ParseIntResult parse_int(const char *string) {
  char *end;
  long value = strtol(string, &end, 10);
  if (*end != '\0')
    return RESULT_ERR(ParseIntResult, "invalid integer");
  return RESULT_OK(ParseIntResult, (int)value);
}

int main(void) {
  ParseIntResult ok = parse_int("123");
  assert(result_is_ok(ok));
  assert(!result_is_err(ok));
  assert(unwrap(ok) == 123);

  ParseIntResult bad = parse_int("12x");
  assert(result_is_err(bad));
  assert(!result_is_ok(bad));
  assert(strcmp(unwrap_err(bad), "invalid integer") == 0);
}
