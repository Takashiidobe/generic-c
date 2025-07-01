#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ListNode ListNode;
struct ListNode {
  ListNode *next;
  char data[];
};

#define List(type)                                                             \
  union {                                                                      \
    ListNode *head;                                                            \
    type *payload;                                                             \
  }

#define list_alloc_front(list)                                                 \
  ((__typeof__((list)->payload))_list_alloc_front(&((list)->head),             \
                                                  sizeof(*(list)->payload)))

#define list_prepend(list, item) *list_alloc_front(list) = item

#define list_data_first(l)                                                     \
  (__typeof__((l)->payload))((l)->head ? (l)->head->data : NULL)
#define list_for(it, l)                                                        \
  for (__typeof__((l)->payload) it = list_data_first(l); it != NULL;           \
       it = list_data_next(it))

void *_list_alloc_front(ListNode **head, size_t data_size) {
  // I recommend using an arena allocator instead of malloc,
  // I use malloc here just for familiarity
  ListNode *node = malloc(sizeof(*node) + data_size);
  assert(node);
  node->next = *head;
  *head = node;
  return node->data;
}

void *list_data_next(void *data) {
  assert(data);
  ListNode *node = data - offsetof(ListNode, data);
  node = node->next;
  return node ? node->data : NULL;
}

typedef struct {
  int value;
} Foo;

int main(int argc, char *argv[]) {
  List(Foo) foo_list = {0};

  list_prepend(&foo_list, (Foo){3});
  list_prepend(&foo_list, (Foo){2});

  // If you wanted to initialize the data in place:
  Foo *new_foo = list_alloc_front(&foo_list);
  new_foo->value = 1;

  // these won't compile, which is good!
  // int *my_int = list_alloc_front(&foo_list);
  // list_prepend(&foo_list, 7);

  list_for(item, &foo_list) {
    // `item` is of type `Foo *`
    printf("%i\n", item->value);
  }

  return 0;
}
