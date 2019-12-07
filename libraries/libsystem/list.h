#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define list_foreach(item, list) for (ListItem *item = list->head; item != NULL; item = item->next)

typedef struct ListItem
{
    void *value;

    // These fields bellow should not be accessed directly.
    struct ListItem *prev;
    struct ListItem *next;
} ListItem;

typedef struct list
{
    // These fields bellow should not be accessed directly.
    int count;
    ListItem *head;
    ListItem *tail;
} List;

// Return true if the two value are ordered
typedef bool (*ListComparator)(void *left, void *right);

/* --- List object constructor and destructor ------------------------------- */

// Create a new list object in memory.
List *list_create();

typedef enum
{
    LIST_KEEP_VALUES,
    LIST_FREE_VALUES,
    LIST_RELEASE_VALUES,
} list_delete_action_t;

// Free the memory used by the list
void list_destroy(List *l, list_delete_action_t free_items);

// Create a copy of the list.
List *list_clone(List *list);

/* --- List operation ------------------------------------------------------- */

// Clear all the item in the list.
void list_clear(List *list, list_delete_action_t free_items);

// Insert a list item sorted.
void list_insert_sorted(List *list, void *value, ListComparator comparator);

// Take a look at the **first item** of the list.
bool list_peek(List *list, void **value);

// Take a look at the **first item** of the list, and push it to the **tail** of the list
bool list_peek_and_pushback(List *list, void **value);

// Take a look at the **last item** of the list.
bool list_peekback(List *list, void **value);

// Take a look at the **last item** of the list, and push it to the **head** of the list
bool list_peekback_and_push(List *list, void **value);

// Take a look at the item at the **position specified by index** in the list.
bool list_peekat(List *list, int index, void **value);

// Push an item to the **head** of the list.
void list_push(List *list, void *value);

// Push an item to the **tail** of the list
void list_pushback(List *list, void *value);

// Pop the head item of the list.
bool list_pop(List *list, void **value);

// Pop the tail item of the list.
bool list_popback(List *list, void **value);

// Check if the list conatain a given item.
bool list_containe(List *list, void *value);

// Remove the first occurence of an item in the list.
bool list_remove(List *list, void *value);

// Is the list empty?
#define list_empty(__list) ((__list)->count == 0)

// Is the list has any item in it?
#define list_any(__list) ((__list)->count != 0)

// Get item count
#define list_count(__list) ((__list)->count)
