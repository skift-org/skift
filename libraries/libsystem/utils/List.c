/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/utils/List.h>

List *list_create(void)
{
    List *list = __create(List);

    list->count = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

void list_destroy(List *list) { list_destroy_with_callback(list, NULL); }
void list_destroy_with_callback(List *list, ListDestroyElementCallback callback)
{
    list_clear_with_callback(list, callback);
    free(list);
}

void list_clear(List *list) { list_clear_with_callback(list, NULL); }
void list_clear_with_callback(List *list, ListDestroyElementCallback callback)
{
    ListItem *current = list->head;

    while (current)
    {
        ListItem *next = current->next;

        if (callback)
        {
            callback(current->value);
        }

        free(current);

        current = next;
    }

    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

List *list_clone(List *list)
{
    List *copy = list_create();

    list_foreach(void, value, list)
    {
        list_pushback(copy, value);
    }

    return copy;
}

void list_insert_sorted(List *list, void *value, ListCompareElementCallback callback)
{
    if (list->head == NULL || callback(value, list->head->value))
    {
        list_push(list, value);
    }
    else
    {
        ListItem *current = list->head;

        while (current->next != NULL && callback(current->next->value, value))
        {
            current = current->next;
        }

        ListItem *item = __create(ListItem);

        item->prev = current;
        item->next = current->next;
        item->value = value;

        if (current->next == NULL)
        {
            list->tail = item;
        }
        else
        {
            current->next->prev = item;
        }

        current->next = item;

        list->count++;
    }
}

bool list_peek(List *list, void **value)
{
    if (list->head != NULL)
    {
        *value = list->head->value;

        return true;
    }
    else
    {
        *value = NULL;

        return false;
    }
}

bool list_peek_and_pushback(List *list, void **value)
{
    if (list_peek(list, value))
    {
        ListItem *item = list->head;

        // Pop
        if (list->count == 1)
        {
            list->count = 0;
            list->head = NULL;
            list->tail = NULL;
        }
        else if (list->count > 1)
        {
            item->next->prev = NULL;
            list->head = item->next;

            list->count--;
        }

        // Push back
        item->prev = NULL;
        item->next = NULL;

        list->count++;

        if (list->tail == NULL)
        {
            list->tail = item;
            list->head = item;
        }
        else
        {
            list->tail->next = item;
            item->prev = list->tail;
            list->tail = item;
        }

        return true;
    }
    else
    {
        return false;
    }
}

bool list_peekback(List *list, void **value)
{
    if (list->tail != NULL)
    {
        *value = list->tail->value;

        return true;
    }
    else
    {
        return false;
    }
}

static void list_peekat_from_head(List *list, int index, void **value)
{
    ListItem *current = list->head;

    for (int i = 0; i < index; i++)
    {
        current = current->next;
    }

    *value = current->value;
}

static void list_peekat_from_back(List *list, int index, void **value)
{
    ListItem *current = list->tail;

    for (int i = 0; i < (list->count - index - 1); i++)
    {
        current = current->prev;
    }

    *value = current->value;
}

bool list_peekat(List *list, int index, void **value)
{
    if (list->count >= 1 && index >= 0 && index < list->count)
    {
        if (index < list->count / 2)
        {
            list_peekat_from_head(list, index, value);
        }
        else
        {
            list_peekat_from_back(list, index, value);
        }

        return true;
    }
    else
    {
        return false;
    }
}

int list_indexof(List *list, void *value)
{
    int index = 0;

    list_foreach(void, item, list)
    {
        if (item == value)
        {
            return index;
        }

        index++;
    }

    return -1;
}

void list_push(List *list, void *value)
{
    ListItem *item = __create(ListItem);

    item->value = value;

    list->count++;

    if (list->head == NULL)
    {
        list->head = item;
        list->tail = item;
    }
    else
    {
        list->head->prev = item;
        item->next = list->head;
        list->head = item;
    }
}

bool list_pop(List *list, void **value)
{
    ListItem *item = list->head;

    if (list->count == 0)
    {
        return false;
    }
    else if (list->count == 1)
    {
        list->count = 0;
        list->head = NULL;
        list->tail = NULL;
    }
    else if (list->count > 1)
    {
        item->next->prev = NULL;
        list->head = item->next;

        list->count--;
    }

    if (value != NULL)
    {
        *value = item->value;
    }

    return true;
}

void list_pushback(List *list, void *value)
{
    ListItem *item = __create(ListItem);

    item->prev = NULL;
    item->next = NULL;
    item->value = value;

    list->count++;

    if (list->tail == NULL)
    {
        list->tail = item;
        list->head = item;
    }
    else
    {
        list->tail->next = item;
        item->prev = list->tail;
        list->tail = item;
    }
}

bool list_popback(List *list, void **value)
{
    ListItem *item = list->tail;

    if (list->count == 0)
    {
        return NULL;
    }
    else if (list->count == 1)
    {
        list->count = 0;
        list->head = NULL;
        list->tail = NULL;
    }
    else if (list->count > 1)
    {
        item->prev->next = NULL;
        list->tail = item->prev;

        list->count--;
    }

    if (value != NULL)
    {
        *value = item->value;
    }

    return true;
}

bool list_remove(List *list, void *value) { return list_remove_with_callback(list, value, NULL); }
bool list_remove_with_callback(List *list, void *value, ListDestroyElementCallback callback)
{
    for (ListItem *item = list->head; item != NULL; item = item->next)
    {
        if (item->value == value)
        {
            if (item->prev != NULL)
            {
                item->prev->next = item->next;
            }
            else
            {
                list->head = item->next;
            }

            if (item->next != NULL)
            {
                item->next->prev = item->prev;
            }
            else
            {
                list->tail = item->prev;
            }

            list->count--;

            if (callback)
            {
                callback(item->value);
            }

            free(item);

            return true;
        }
    }

    return false;
}

bool list_contains(List *list, void *value)
{
    list_foreach(void, item, list)
    {
        if (item == value)
        {
            return true;
        }
    }

    return false;
}