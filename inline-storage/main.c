#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int a;
} Foo;

typedef struct {
  int x;
  int y;
} Vec;

typedef struct ListNode ListNode;
struct ListNode {
  ListNode *next;
  char data[]; // glossing over some padding/alignment details here
};

void list_prepend(ListNode **head, void *data, size_t data_size) {
  ListNode *node = malloc(sizeof(*node) + data_size);
  memcpy(node->data, data, data_size);
  node->next = *head;
  *head = node;
}

int main() {
  ListNode *foo_list = NULL;
  Foo foo = {5};
  list_prepend(&foo_list, &foo, sizeof(foo));

  Foo *fp = (Foo *)foo_list->data;

  printf("foo head: %d\n", fp->a);

  ListNode *vec_list = NULL;
  Vec vec = {5, 10};
  list_prepend(&vec_list, &vec, sizeof(vec));
  Vec *vp = (Vec *)vec_list->data;

  printf("vec x: %d, vec y: %d\n", vp->x, vp->y);
}
