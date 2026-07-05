#include "result.h"
#include "../test/assert.h"

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
  GC_ASSERT(result_is_ok(ok));
  GC_ASSERT(!result_is_err(ok));
  GC_ASSERT(unwrap(ok) == 123);

  ParseIntResult bad = parse_int("12x");
  GC_ASSERT(result_is_err(bad));
  GC_ASSERT(!result_is_ok(bad));
  GC_ASSERT(strcmp(unwrap_err(bad), "invalid integer") == 0);
}
