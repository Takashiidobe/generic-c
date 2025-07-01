#include <stdlib.h>

typedef struct ListNode ListNode;

struct ListNode {
  ListNode *next;
  void *data;
};

void list_prepend(ListNode **head, void *data) {
  ListNode *node = malloc(sizeof(*node));
  node->data = data;
  node->next = *head;
  *head = node;
}
