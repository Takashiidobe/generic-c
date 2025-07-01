#ifndef T
#error "T must be defined before including this header"
#endif

#include <stdlib.h>

#define _CONCAT(a, b) a##b
#define CONCAT(a, b) _CONCAT(a, b)

#define NODE_TYPE CONCAT(T, ListNode)
#define PREPEND_FUNC CONCAT(T, _list_prepend)

typedef struct NODE_TYPE NODE_TYPE;
struct NODE_TYPE {
  NODE_TYPE *next;
  T data;
};

void PREPEND_FUNC(NODE_TYPE **head, T data) {
  NODE_TYPE *node = malloc(sizeof(*node));
  node->data = data;
  node->next = *head;
  *head = node;
}

#undef T
#undef _CONCAT
#undef CONCAT
#undef NODE_TYPE
#undef PREPEND_FUNC
