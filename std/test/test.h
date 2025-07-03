#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

// ——— global counters (one per translation unit) ———
static int _tests_run = 0;
static int _tests_failed = 0;

// ——— begin a new test case ———
// Usage:
//   TEST("adding two numbers") {
//     int sum = add(2,3);
//     ASSERT_EQ(sum, 5);
//   }
#define TEST(desc)                                                             \
  for (int _run = (_tests_run++, printf("[TEST] %s\n", desc), 1); _run;        \
       _run = 0)

// ——— basic assertion ———
// fails if expr is false
#define ASSERT(expr)                                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr, "%s:%d: Assertion failed: %s\n", __FILE__, __LINE__,     \
              #expr);                                                          \
      _tests_failed++;                                                         \
    }                                                                          \
  } while (0)

// ——— equality ———
// fails if a != b
#define ASSERT_EQ(a, b)                                                        \
  do {                                                                         \
    typeof(a) _a = (a);                                                        \
    typeof(b) _b = (b);                                                        \
    if (memcmp(&_a, &_b, sizeof _a) != 0) {                                    \
      fprintf(stderr, "%s:%d: ASSERT_EQ failed: %s == %s (bytes differ)\n",    \
              __FILE__, __LINE__, #a, #b);                                     \
      _tests_failed++;                                                         \
    }                                                                          \
  } while (0)

// ——— inequality ———
// fails if a == b
#define ASSERT_NE(a, b)                                                        \
  do {                                                                         \
    typeof(a) _a = (a);                                                        \
    typeof(b) _b = (b);                                                        \
    if (!(_a != _b)) {                                                         \
      fprintf(stderr, "%s:%d: ASSERT_NE failed: %s != %s (both %lld)\n",       \
              __FILE__, __LINE__, #a, #b, (long long)_a);                      \
      _tests_failed++;                                                         \
    }                                                                          \
  } while (0)

// ——— print a summary and exit non-zero on failure ———
#define TEST_SUMMARY()                                                         \
  do {                                                                         \
    printf("Tests run: %d; Failures: %d\n", _tests_run, _tests_failed);        \
    if (_tests_failed)                                                         \
      exit(EXIT_FAILURE);                                                      \
  } while (0)

#endif // TEST_H
