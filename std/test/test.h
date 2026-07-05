#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ——— global counters (one per translation unit) ———
static int gc_tests_run = 0;
static int gc_tests_failed = 0;

// ——— begin a new test case ———
// Usage:
//   TEST("adding two numbers") {
//     int sum = add(2,3);
//     ASSERT_EQ(sum, 5);
//   }
#define TEST(desc)                                                             \
  for (int _run = (gc_tests_run++, printf("[TEST] %s\n", desc), 1); _run;        \
       _run = 0)

// ——— basic assertion ———
// fails if expr is false
#define ASSERT(expr)                                                           \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr, "%s:%d: Assertion failed: %s\n", __FILE__, __LINE__,     \
              #expr);                                                          \
      gc_tests_failed++;                                                         \
    }                                                                          \
  } while (0)

// ——— string equality ———
// fails if strcmp(a, b) != 0
#define ASSERT_STR_EQ(a, b)                                                    \
  do {                                                                         \
    const char *_a = (a);                                                      \
    const char *_b = (b);                                                      \
    if (strcmp(_a, _b) != 0) {                                                 \
      fprintf(stderr, "%s:%d: ASSERT_STR_EQ failed: \"%s\" != \"%s\"\n",       \
              __FILE__, __LINE__, _a, _b);                                     \
      gc_tests_failed++;                                                         \
    }                                                                          \
  } while (0)

// ——— equality ———
// compares raw bytes; only reliable for scalars or fully-initialized structs
// (padding bytes are compared too). use ASSERT_STR_EQ for strings.
#define ASSERT_EQ(a, b)                                                        \
  do {                                                                         \
    typeof(a) _a = (a);                                                        \
    typeof(b) _b = (b);                                                        \
    if (memcmp(&_a, &_b, sizeof _a) != 0) {                                    \
      fprintf(stderr, "%s:%d: ASSERT_EQ failed: %s == %s (bytes differ)\n",    \
              __FILE__, __LINE__, #a, #b);                                     \
      gc_tests_failed++;                                                         \
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
      gc_tests_failed++;                                                         \
    }                                                                          \
  } while (0)

// ——— print a summary and exit non-zero on failure ———
#define TEST_SUMMARY()                                                         \
  do {                                                                         \
    printf("Tests run: %d; Failures: %d\n", gc_tests_run, gc_tests_failed);        \
    if (gc_tests_failed)                                                         \
      exit(EXIT_FAILURE);                                                      \
  } while (0)

#endif // TEST_H
