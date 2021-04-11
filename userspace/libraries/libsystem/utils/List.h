#pragma once

#include <libsystem/Common.h>
#include <libutils/Iteration.h>

#define list_foreach(__type, __item, __list)                             \
    for (ListItem *__i = __list->_head; __i != nullptr; __i = __i->next) \
        for (__type *__item = (__type *)__i->value, *__loop_once = (__type *)1; __loop_once; __loop_once = 0)

#define list_foreach_reversed(__type, __item, __list)                    \
    for (ListItem *__i = __list->_tail; __i != nullptr; __i = __i->prev) \
        for (__type *__item = (__type *)__i->value, *__loop_once = (__type *)1; __loop_once; __loop_once = 0)

struct ListItem
{
    void *value;

    struct ListItem *prev;
    struct ListItem *next;
};

struct List
{
    int _count;
    ListItem *_head;
    ListItem *_tail;

    auto empty() { return _count == 0; }

    auto any() { return _count > 0; }

    auto count() { return _count; }
};

typedef bool (*ListCompareElementCallback)(void *left, void *right);
typedef void (*ListDestroyElementCallback)(void *element);

List *list_create();

void list_destroy(List *list);

void list_destroy_with_callback(List *list, ListDestroyElementCallback callback);

void list_clear(List *list);

void list_clear_with_callback(List *list, ListDestroyElementCallback callback);

List *list_clone(List *list);

void list_insert_sorted(List *list, void *value, ListCompareElementCallback comparator);

void *list_peek(List *list);

bool list_peek_and_pushback(List *list, void **value);

void *list_peekback(List *list);

bool list_peekat(List *list, int index, void **value);

int list_indexof(List *list, void *value);

void list_insert(List *list, int index, void *value);

void list_push(List *list, void *value);

void list_pushback(List *list, void *value);

void list_pushback_copy(List *list, void *value, size_t size);

bool list_pop(List *list, void **value);

bool list_popback(List *list, void **value);

bool list_contains(List *list, void *value);

bool list_remove(List *list, void *value);

bool list_remove_with_callback(List *list, void *value, ListDestroyElementCallback callback);

bool list_remove_at(List *list, int index);

bool list_remove_at_with_callback(List *list, int index, ListDestroyElementCallback callback);

typedef Iteration (*ListIterationCallback)(void *target, void *value);

bool list_iterate(List *list, void *target, ListIterationCallback callback);
