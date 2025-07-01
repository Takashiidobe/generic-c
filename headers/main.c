#include <stdio.h>
typedef struct {
  int a;
} Foo;

typedef struct {
  char *str;
  double num;
} Bar;

#define T Foo
#include "list.h"

#define T Bar
#include "list.h"

int main() {
  FooListNode *foo_head = NULL;
  Foo_list_prepend(&foo_head, (Foo){1});

  printf("foo_head.a = %d\n", foo_head->data.a);

  BarListNode *bar_head = NULL;
  Bar_list_prepend(&bar_head, (Bar){"hello", 5.4});

  printf("bar_head.str = %s, bar_head.num = %f\n", bar_head->data.str,
         bar_head->data.num);
}
