#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define list_foreach(__type, __item, __list)                         \
    for (ListItem *__i = __list->head; __i != NULL; __i = __i->next) \
        for (__type *__item = (__type *)__i->value, *__loop_once = (__type *)1; __loop_once; __loop_once = 0)

typedef struct ListItem
{
    void *value;

    struct ListItem *prev;
    struct ListItem *next;
} ListItem;

typedef struct List
{
    int count;

    ListItem *head;
    ListItem *tail;
} List;

typedef bool (*ListCompareElementCallback)(void *left, void *right);
typedef void (*ListDestroyElementCallback)(void *element);

List *list_create(void);

void list_destroy(List *list);

void list_destroy_with_callback(List *list, ListDestroyElementCallback callback);

void list_clear(List *list);

void list_clear_with_callback(List *list, ListDestroyElementCallback callback);

List *list_clone(List *list);

void list_insert_sorted(List *list, void *value, ListCompareElementCallback comparator);

bool list_peek(List *list, void **value);

bool list_peek_and_pushback(List *list, void **value);

bool list_peekback(List *list, void **value);

bool list_peekat(List *list, int index, void **value);

int list_indexof(List *list, void *value);

void list_push(List *list, void *value);

void list_pushback(List *list, void *value);

bool list_pop(List *list, void **value);

bool list_popback(List *list, void **value);

bool list_contains(List *list, void *value);

bool list_remove(List *list, void *value);

bool list_remove_with_callback(List *list, void *value, ListDestroyElementCallback callback);

#define list_empty(__list) ((__list)->count == 0)

#define list_any(__list) ((__list)->count != 0)

#define list_count(__list) ((__list)->count)
