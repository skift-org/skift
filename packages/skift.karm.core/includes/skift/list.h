#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/types.h>
#include <skift/utils.h>

#define FOREACH(item, list) for (list_item_t *item = list->head; item != NULL; item = item->next)
#define FOREACHR(item, list) for (list_item_t *item = list->tail; item != NULL; item = item->prev)

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

typedef bool (*list_comparator_t)(void* left, void* right); // return true if the two value are ordered

list_t *list();

void list_delete(list_t *l); // Free the data structure, BUT doesn't free the childs.
void list_destroy(list_t *list); // Free the data structure, AND free the childs.
void list_clear(list_t *list);

void list_insert_sorted(list_t* list, void* value, list_comparator_t comparator);

bool list_peek(list_t* list, void** value);
bool list_peekback(list_t* list, void** value);

void list_push(list_t *list, void *value);
int list_pop(list_t *list, void **value);

void list_pushback(list_t *list, void *value);
int list_popback(list_t *list, void **value);

int list_remove(list_t *list, void *value); // Remove the first occurence of a value in the list.
bool list_containe(list_t *list, void *value);

#define list_empty(__list) ((__list)->count == 0)
