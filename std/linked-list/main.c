#include "list.h"
#include <stddef.h>
#include <stdio.h>

typedef struct {
  int value;
} Foo;

int main(void) {
  // Declare and initialize a List of Foo
  List(Foo) list = {0};
  list_init(&list);

  // Prepend some values
  list_prepend(&list, (Foo){.value = 10});
  list_prepend(&list, (Foo){.value = 20});
  list_prepend(&list, (Foo){.value = 30});

  // Append some values
  list_append(&list, (Foo){.value = 40});
  list_append(&list, (Foo){.value = 50});

  // Insert in-place at front and back
  Foo *fp = list_alloc_front(&list);
  fp->value = 5;
  Foo *bp = list_alloc_back(&list);
  bp->value = 100;

  // Iterate forward
  printf("Forward iteration:\n");
  Foo *it;
  list_for(it, &list) { printf("  %d\n", it->value); }

  // Iterate backward
  printf("Backward iteration:\n");
  list_for_rev(it, &list) { printf("  %d\n", it->value); }

  // Pop front and back
  Foo front = list_pop_front(&list);
  Foo back = list_pop_back(&list);
  printf("Popped front: %d, Popped back: %d\n", front.value, back.value);

  // Final forward iteration
  printf("After pops, remaining:\n");
  list_for(it, &list) { printf("  %d\n", it->value); }

  // Clear all nodes
  // Since head is first field, offset is 0
  list_clear(&list, 0);

  // Check empty
  if (list_is_empty(&list)) {
    printf("List is now empty.\n");
  } else {
    printf("List not empty!\n");
  }

  return 0;
}
