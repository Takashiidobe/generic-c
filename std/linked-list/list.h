#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

// ─── Node definition (doubly linked, generic payload) ────────────────────
typedef struct ListNode {
  struct ListNode *prev, *next;
  char data[]; // flexible array for payload
} ListNode;

// ─── List container type for T ──────────────────────────────────────────
// Declare a List of T as: List(int) mylist = {0};
#define List(T)                                                                \
  struct {                                                                     \
    ListNode *head, *tail;                                                     \
    T *payload;                                                                \
  }

// ─── initialize an empty list ───────────────────────────────────────────
#define list_init(lst)                                                         \
  do {                                                                         \
    (lst)->head = (lst)->tail = NULL;                                          \
  } while (0)

// ─── allocate and insert at front ───────────────────────────────────────
static inline void *_list_alloc_front(ListNode **head, ListNode **tail,
                                      size_t data_size) {
  ListNode *node = malloc(sizeof *node + data_size);
  assert(node);
  node->prev = NULL;
  node->next = *head;
  if (*head)
    (*head)->prev = node;
  *head = node;
  if (*tail == NULL)
    *tail = node;
  return node->data;
}

// ─── allocate and insert at back ────────────────────────────────────────
static inline void *_list_alloc_back(ListNode **head, ListNode **tail,
                                     size_t data_size) {
  ListNode *node = malloc(sizeof *node + data_size);
  assert(node);
  node->next = NULL;
  node->prev = *tail;
  if (*tail)
    (*tail)->next = node;
  *tail = node;
  if (*head == NULL)
    *head = node;
  return node->data;
}

// ─── prepend value ──────────────────────────────────────────────────────
#define list_prepend(lst, val)                                                 \
  do {                                                                         \
    __typeof__((lst)->payload) _tmp = list_alloc_front(lst);                   \
    *_tmp = (val);                                                             \
  } while (0)

// ─── append value ───────────────────────────────────────────────────────
#define list_append(lst, val)                                                  \
  do {                                                                         \
    __typeof__((lst)->payload) _tmp = list_alloc_back(lst);                    \
    *_tmp = (val);                                                             \
  } while (0)

// ─── allocate in-place at front ─────────────────────────────────────────
#define list_alloc_front(lst)                                                  \
  (__typeof__((lst)->payload))_list_alloc_front(                               \
      &((lst)->head), &((lst)->tail), sizeof *(lst)->payload)

// ─── allocate in-place at back ──────────────────────────────────────────
#define list_alloc_back(lst)                                                   \
  (__typeof__((lst)->payload))_list_alloc_back(&((lst)->head), &((lst)->tail), \
                                               sizeof *(lst)->payload)

// ─── pop from front (returns T) ──────────────────────────────────────────
#define list_pop_front(lst)                                                    \
  ({                                                                           \
    ListNode **_h = &(lst)->head;                                              \
    ListNode **_t = &(lst)->tail;                                              \
    assert(*_h);                                                               \
    ListNode *_n = *_h;                                                        \
    __typeof__((lst)->payload) _pd = (lst)->payload;                           \
    __typeof__(*(_pd)) _res = *(__typeof__(*(_pd)) *)_n->data;                 \
    *_h = _n->next;                                                            \
    if (*_h)                                                                   \
      (*_h)->prev = NULL;                                                      \
    else                                                                       \
      *(_t) = NULL;                                                            \
    free(_n);                                                                  \
    _res;                                                                      \
  })

// ─── pop from back (returns T) ───────────────────────────────────────────
#define list_pop_back(lst)                                                     \
  ({                                                                           \
    ListNode **_h = &(lst)->head;                                              \
    ListNode **_t = &(lst)->tail;                                              \
    assert(*_t);                                                               \
    ListNode *_n = *_t;                                                        \
    __typeof__((lst)->payload) _pd = (lst)->payload;                           \
    __typeof__(*(_pd)) _res = *(__typeof__(*(_pd)) *)_n->data;                 \
    *_t = _n->prev;                                                            \
    if (*_t)                                                                   \
      (*_t)->next = NULL;                                                      \
    else                                                                       \
      *(_h) = NULL;                                                            \
    free(_n);                                                                  \
    _res;                                                                      \
  })

// ─── check if empty ─────────────────────────────────────────────────────
#define list_is_empty(lst) (((lst)->head) == NULL)

// ─── iterate forward: it is T* ──────────────────────────────────────────
#define list_for(it, lst)                                                      \
  for (__typeof__((lst)->payload) it =                                         \
           ((lst)->head ? (__typeof__((lst)->payload))(lst)->head->data        \
                        : NULL);                                               \
       it != NULL;                                                             \
       it = (__extension__({                                                   \
         ListNode *_n = (ListNode *)((char *)it - offsetof(ListNode, data));   \
         _n->next ? (__typeof__((lst)->payload))_n->next->data : NULL;         \
       })))

// ─── iterate backward: it is T* ─────────────────────────────────────────
#define list_for_rev(it, lst)                                                  \
  for (__typeof__((lst)->payload) it =                                         \
           ((lst)->tail ? (__typeof__((lst)->payload))(lst)->tail->data        \
                        : NULL);                                               \
       it != NULL;                                                             \
       it = (__extension__({                                                   \
         ListNode *_n = (ListNode *)((char *)it - offsetof(ListNode, data));   \
         _n->prev ? (__typeof__((lst)->payload))_n->prev->data : NULL;         \
       })))

// ─── clear all nodes ─────────────────────────────────────────────────────
static inline void list_clear(void *lst, size_t offset) {
  char *base = (char *)lst;
  ListNode *n = *(ListNode **)(base + offset);
  while (n) {
    ListNode *nx = n->next;
    free(n);
    n = nx;
  }
}

#endif // LINKED_LIST_H
