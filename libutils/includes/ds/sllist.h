#pragma once
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct node
{
  int data;
  struct node *next;
} sllist_node_t;

typedef struct list
{
  uint count;
  sllist_node_t *head;
} sllist_t;

sllist_t *sll_new();
void sll_display(sllist_t *list);

void sll_add(int data, sllist_t *list);
void sll_delete(int data, sllist_t *list);
sllist_node_t* sll_index(sllist_t *list, uint index);
void sll_reverse(sllist_t *list);

void sll_free(sllist_t *list);
void sll_destroy(sllist_t *list);

#define SLL_FOREARCH(x, list) for (sllist_node_t *x = list->head; x != NULL; x = x->next)
