#include "list.h"

typedef struct {
  int value;
} Foo;

int main(void) {
  List(Foo) list = {0};
  list_init(&list);
  assert(list_is_empty(&list));

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
  list_for(item, &list) { assert(item->value == forward[index++]); }
  assert(index == 7);

  const int reverse[] = {100, 50, 40, 10, 20, 30, 5};
  index = 0;
  list_for_rev(item, &list) { assert(item->value == reverse[index++]); }
  assert(index == 7);

  Foo front = list_pop_front(&list);
  Foo back = list_pop_back(&list);
  assert(front.value == 5);
  assert(back.value == 100);

  const int remaining[] = {30, 20, 10, 40, 50};
  index = 0;
  list_for(item, &list) { assert(item->value == remaining[index++]); }
  assert(index == 5);

  list_clear(&list);
  assert(list_is_empty(&list));
  assert(list.tail == NULL);
}
