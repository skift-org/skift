#pragma once

#include <skift/types.h>
#include <skift/utils.h>

#define FOREACH(item, list) for (list_item_t *item = list->head; item != NULL; item = item->next)

typedef struct list_node
{
    struct list_node *prev;
    struct list_node *next;
    void *value;
} list_item_t;

typedef struct list
{
    int count;
    list_item_t *head;
    list_item_t *tail;
} list_t;

list_t *list();
void list_delete(list_t *l);
void list_destroy(list_t *list);

void list_push(list_t *list, void *value);
int list_pop(list_t *list, void **value);

void list_pushback(list_t *list, void *value);
int list_popback(list_t *list, void **value);

/** Remove the first occurence of a value in the list.*/
int list_remove(list_t *list, void *value);
int list_containe(list_t *list, void *value);
