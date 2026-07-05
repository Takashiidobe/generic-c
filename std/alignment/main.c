#include "../array/vector.h"
#include "../btreemap/btreemap.h"
#include "../btreeset/btreeset.h"
#include "../hashmap/hashmap.h"
#include "../hashset/hashset.h"
#include "../linked-list/list.h"
#include "../vecdeque/vecdeque.h"

#include <assert.h>
#include <stdint.h>

#define ASSERT_ALIGNED(ptr)                                                    \
  assert((uintptr_t)(ptr) % _Alignof(max_align_t) == 0)

int main(void) {
  Vec(max_align_t) vec = NULL;
  vec_push(vec, (max_align_t){0});
  ASSERT_ALIGNED(vec);
  vec_free(vec);

  Deque(max_align_t) deque = NULL;
  dq_push_back(deque, (max_align_t){0});
  ASSERT_ALIGNED(deque);
  dq_free(deque);

  BMap(long double, max_align_t) bmap;
  bmap_init(bmap, 1);
  bmap_put(bmap, 1.0L, (max_align_t){0});
  ASSERT_ALIGNED(bmap_get(bmap, 1.0L));
  bmap_free(bmap);

  BSet(long double) bset;
  bset_init(bset, 1);
  bset_add(bset, 1.0L);
  ASSERT_ALIGNED(gc_ss_keyptr(bset, 0));
  bset_free(bset);

  Map(max_align_t, max_align_t) map;
  map_init(map, 1);
  max_align_t key = {0};
  map_put(map, key, (max_align_t){0});
  ASSERT_ALIGNED(map_get(map, key));
  map_free(map);

  Set(max_align_t) set;
  set_init(set, 1);
  set_add(set, key);
  ASSERT_ALIGNED(set.h->buckets[0]->data);
  set_free(set);

  List(max_align_t) list = {0};
  list_init(&list);
  max_align_t *item = list_alloc_back(&list);
  ASSERT_ALIGNED(item);
  (void)list_pop_back(&list);
}
