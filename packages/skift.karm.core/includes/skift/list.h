#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/types.h>
#include <skift/utils.h>

#define FOREACH(item, list) for (list_item_t *item = list->head; item != NULL; item = item->next)
#define FOREACHR(item, list) for (list_item_t *item = list->tail; item != NULL; item = item->prev)

typedef struct list_node
{
    void *value;

    // These fields bellow should not be accessed directly.
    struct list_node *prev;
    struct list_node *next;
} list_item_t;

typedef struct list
{
    // These fields bellow should not be accessed directly.
    int count;
    list_item_t *head;
    list_item_t *tail;
} list_t;

typedef bool (*list_comparator_t)(void* left, void* right); // return true if the two value are ordered

/* --- List object constructor and destructor ------------------------------- */

// Create a new list object in memory.
list_t *list();

#define LIST_KEEP_VALUES false
#define LIST_FREE_VALUES true

// Free the memory used by the list
void list_delete(list_t *l, bool free_items); 

/* --- List operation ------------------------------------------------------- */

// Clear all the item in the list.
void list_clear(list_t *list, bool free_items);

// Insert a list item sorted.
void list_insert_sorted(list_t* list, void* value, list_comparator_t comparator);

// Take a look at the first item of the list.
bool list_peek(list_t* list, void** value);

// Take a look at the last item of the list.
bool list_peekback(list_t* list, void** value);

// Push an item to the head of the list.
void list_push(list_t *list, void *value);

// Push an item to the tail of the list
void list_pushback(list_t *list, void *value);

// Pop the head item of the list.
bool list_pop(list_t *list, void **value);

// Pop the tail item of the list.
bool list_popback(list_t *list, void **value);

// Check if the list conatain a given item.
bool list_containe(list_t *list, void *value);

// Remove the first occurence of an item in the list.
bool list_remove(list_t *list, void *value); 

// Is the list empty?
#define list_empty(__list) ((__list)->count == 0)

// Is the list has any item in it?
#define list_any(__list) ((__list)->count != 0)

/* --- List node operation -------------------------------------------------- */
// For zero alloc and free list operation this is greate when memory allocation
// are to slow (see: skift.hjert.kernel::sheduler.c).

// bool list_node_peek(list_t* list, list_item_t** value);
// bool list_node_peekback(list_t* list, list_item_t** value);
// 
// void list_node_push(list_t *list, list_item_t *value);
// void list_node_pushback(list_t *list, list_item_t *value);
// 
// bool  list_node_pop(list_t *list, list_item_t **value);
// bool  list_node_popback(list_t *list, list_item_t **value);
