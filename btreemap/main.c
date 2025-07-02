#include "btreemap.h"
#include <stdio.h>
#include <stdlib.h>

void test1() {
  // Declare an empty sorted map: int -> const char*
  SortedMap(int, const char *) m = NULL;

  // Insert some key/value pairs (unordered insertion)
  sorted_map_put(m, int, const char *, 3, "three");
  sorted_map_put(m, int, const char *, 1, "one");
  sorted_map_put(m, int, const char *, 2, "two");
  // Overwrite existing key
  sorted_map_put(m, int, const char *, 2, "TWO");

  // Query length and capacity
  printf("Map length: %zu\n", sm_len(m));
  printf("Map capacity: %zu\n", sm_cap(m));

  // Lookup a value
  const char **val = sorted_map_get(m, int, const char *, 2);
  if (val) {
    printf("Key 2 -> %s\n", *val);
  } else {
    printf("Key 2 not found\n");
  }

  // Iterate in sorted order
  printf("Iterating map in ascending key order:\n");
  Pair(int, const char *) * it;
  sorted_map_for(it, m) { printf("  %d => %s\n", it->key, it->val); }

  // Clean up
  sm_free(m);
}

void test2() {
  // Declare an empty sorted map: int -> const char*
  SortedMap(int, int) m = NULL;

  // Insert some key/value pairs (unordered insertion)
  sorted_map_put(m, int, int, 3, 3);
  sorted_map_put(m, int, int, 1, 1);
  sorted_map_put(m, int, int, 2, 2);
  // Overwrite existing key
  sorted_map_put(m, int, int, 2, 2);

  // Query length and capacity
  printf("Map length: %zu\n", sm_len(m));
  printf("Map capacity: %zu\n", sm_cap(m));

  // Lookup a value
  int *val = sorted_map_get(m, int, int, 2);
  if (val) {
    printf("Key 2 -> %d\n", *val);
  } else {
    printf("Key 2 not found\n");
  }

  // Iterate in sorted order
  printf("Iterating map in ascending key order:\n");
  Pair(int, const char *) * it;
  sorted_map_for(it, m) { printf("  %d => %d\n", it->key, it->val); }

  // Clean up
  sm_free(m);
}

int main() {
  test1();
  test2();
}
