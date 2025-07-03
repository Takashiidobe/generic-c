#include "result.h"
#include <stdio.h>
#include <stdlib.h> // for strtol

typedef Result(int, const char *) ParseIntResult;

// Declare the function with generic Result:
static ParseIntResult parse_int(const char *s) {
  char *end;
  long val = strtol(s, &end, 10);
  if (*end != '\0')
    return (ParseIntResult){.is_ok = false, .as.err = "invalid integer"};
  return (ParseIntResult){.is_ok = true, .as.ok = val};
}

int main(void) {
  ParseIntResult r = parse_int("123");
  printf("Parsed: %d\n", unwrap_err(r));
  return 0;
}
