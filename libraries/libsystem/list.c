/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/list.h>

List *list_create()
{
    List *l = __malloc(List);

    l->count = 0;
    l->head = NULL;
    l->tail = NULL;

    return l;
}

void list_destroy(List *l, list_delete_action_t free_items)
{
    list_clear(l, free_items);
    free(l);
}

List *list_clone(List *Listo_copy)
{
    List *copy = list_create();

    list_foreach(void, value, Listo_copy)
    {
        list_pushback(copy, value);
    }

    return copy;
}

void list_clear(List *list, list_delete_action_t free_items)
{
    ListItem *current = list->head;

    while (current)
    {
        ListItem *next = current->next;

        if (free_items == LIST_FREE_VALUES)
        {
            free(current->value);
        }
        else if (free_items == LIST_RELEASE_VALUES)
        {
            object_release(current->value);
        }

        free(current);
        current = next;
    }

    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_insert_sorted(List *list, void *value, ListComparator comparator)
{
    if (list->head == NULL || comparator(value, list->head->value))
    {
        list_push(list, value);
    }
    else
    {
        ListItem *current = list->head;

        while (current->next != NULL && comparator(current->next->value, value))
        {
            current = current->next;
        }

        ListItem *item = __malloc(ListItem);

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

bool list_peek_and_pushback(List *l, void **value)
{
    if (list_peek(l, value))
    {
        ListItem *item = l->head;

        // Pop
        if (l->count == 1)
        {
            l->count = 0;
            l->head = NULL;
            l->tail = NULL;
        }
        else if (l->count > 1)
        {
            item->next->prev = NULL;
            l->head = item->next;

            l->count--;
        }

        // Push back
        item->prev = NULL;
        item->next = NULL;

        l->count++;

        if (l->tail == NULL)
        {
            l->tail = item;
            l->head = item;
        }
        else
        {
            l->tail->next = item;
            item->prev = l->tail;
            l->tail = item;
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

bool list_peekat(List *list, int index, void **value)
{
    if (index < list->count)
    {
        ListItem *current = list->head;

        for (int i = 0; i < index; i++)
        {
            current = current->next;
        }

        *value = current->value;

        return true;
    }
    else
    {
        return false;
    }
}

void list_push(List *l, void *value)
{
    ListItem *item = __malloc(ListItem);

    item->prev = NULL;
    item->next = NULL;
    item->value = value;

    l->count++;

    if (l->head == NULL)
    {
        l->head = item;
        l->tail = item;
    }
    else
    {
        l->head->prev = item;
        item->next = l->head;
        l->head = item;
    }
}

bool list_pop(List *l, void **value)
{
    ListItem *item = l->head;

    if (l->count == 0)
    {
        return false;
    }
    else if (l->count == 1)
    {
        l->count = 0;
        l->head = NULL;
        l->tail = NULL;
    }
    else if (l->count > 1)
    {
        item->next->prev = NULL;
        l->head = item->next;

        l->count--;
    }

    if (value != NULL)
        *(value) = item->value;

    free(item);

    return true;
}

void list_pushback(List *l, void *value)
{
    ListItem *item = __malloc(ListItem);

    item->prev = NULL;
    item->next = NULL;
    item->value = value;

    l->count++;

    if (l->tail == NULL)
    {
        l->tail = item;
        l->head = item;
    }
    else
    {
        l->tail->next = item;
        item->prev = l->tail;
        l->tail = item;
    }
}

bool list_popback(List *l, void **value)
{
    ListItem *item = l->tail;

    if (l->count == 0)
    {
        return NULL;
    }
    else if (l->count == 1)
    {
        l->count = 0;
        l->head = NULL;
        l->tail = NULL;
    }
    else if (l->count > 1)
    {
        item->prev->next = NULL;
        l->tail = item->prev;

        l->count--;
    }

    if (value != NULL)
        *(value) = item->value;

    free(item);

    return true;
}

bool list_remove(List *this, void *value)
{
    for (ListItem *item = this->head; item != NULL; item = item->next)
    {
        if (item->value == value)
        {
            if (item->prev != NULL)
            {
                item->prev->next = item->next;
            }
            else
            {
                this->head = item->next;
            }

            if (item->next != NULL)
            {
                item->next->prev = item->prev;
            }
            else
            {
                this->tail = item->prev;
            }

            this->count--;
            free(item);

            return true;
        }
    }

    return false;
}

bool list_containe(List *l, void *value)
{
    list_foreach(void, item, l)
    {
        if (item == value)
        {
            return true;
        }
    }

    return false;
}