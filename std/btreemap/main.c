#include "btreemap.h"
#include "../test/assert.h"

#include <string.h>

typedef struct {
  int value;
} Foo;

typedef struct {
  const char *value;
} Bar;

int main(void) {
  BMap(int, Foo) map;
  bmap_init(map, 4);

  bmap_put(map, 3, (Foo){.value = 30});
  bmap_put(map, 1, (Foo){.value = 10});
  bmap_put(map, 2, (Foo){.value = 20});
  bmap_put(map, 2, (Foo){.value = 200});
  bmap_put(map, 4, (Foo){.value = 44});

  GC_ASSERT(sm_len(map) == 4);
  GC_ASSERT(bmap_get(map, 2)->value == 200);
  GC_ASSERT(bmap_get(map, 99) == NULL);
  GC_ASSERT(bmap_remove(map, 2));
  GC_ASSERT(!bmap_remove(map, 2));
  GC_ASSERT(sm_len(map) == 3);

  const int expected_keys[] = {1, 3, 4};
  const int expected_values[] = {10, 30, 44};
  size_t index = 0;
  bmap_for(map, key, value) {
    GC_ASSERT(key == expected_keys[index]);
    GC_ASSERT(value.value == expected_values[index]);
    ++index;
  }
  GC_ASSERT(index == 3);
  bmap_free(map);
  GC_ASSERT(map.h == NULL);

  BMap(const char *, Bar) strings;
  bmap_init_with(strings, 1, gc_str_cmp);
  bmap_put(strings, "k", (Bar){.value = "xd"});
  bmap_put(strings, "k2", (Bar){.value = "xdd"});
  bmap_put(strings, "k1", (Bar){.value = "xddd"});

  const char *ordered_keys[] = {"k", "k1", "k2"};
  const char *ordered_values[] = {"xd", "xddd", "xdd"};
  index = 0;
  bmap_for(strings, key, value) {
    GC_ASSERT(strcmp(key, ordered_keys[index]) == 0);
    GC_ASSERT(strcmp(value.value, ordered_values[index]) == 0);
    ++index;
  }
  GC_ASSERT(index == 3);
  bmap_free(strings);
}
