#include "list.h"
#include "../test/assert.h"

typedef struct {
  int value;
} Foo;

int main(void) {
  List(Foo) list = {0};
  list_init(&list);
  GC_ASSERT(list_is_empty(&list));

  list_prepend(&list, (Foo){.value = 10});
  list_prepend(&list, (Foo){.value = 20});
  list_prepend(&list, (Foo){.value = 30});
  list_append(&list, (Foo){.value = 40});
  list_append(&list, (Foo){.value = 50});

  Foo *front_item = list_alloc_front(&list);
  front_item->value = 5;
  Foo *back_item = list_alloc_back(&list);
  back_item->value = 100;

  const int forward[] = {5, 30, 20, 10, 40, 50, 100};
  size_t index = 0;
  list_for(item, &list) { GC_ASSERT(item->value == forward[index++]); }
  GC_ASSERT(index == 7);

  const int reverse[] = {100, 50, 40, 10, 20, 30, 5};
  index = 0;
  list_for_rev(item, &list) { GC_ASSERT(item->value == reverse[index++]); }
  GC_ASSERT(index == 7);

  Foo front = list_pop_front(&list);
  Foo back = list_pop_back(&list);
  GC_ASSERT(front.value == 5);
  GC_ASSERT(back.value == 100);

  const int remaining[] = {30, 20, 10, 40, 50};
  index = 0;
  list_for(item, &list) { GC_ASSERT(item->value == remaining[index++]); }
  GC_ASSERT(index == 5);

  list_clear(&list);
  GC_ASSERT(list_is_empty(&list));
  GC_ASSERT(list.tail == NULL);
}
