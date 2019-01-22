/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdlib.h>
#include <stdio.h>

#include <skift/list.h>

list_t *list()
{
    list_t *l = malloc(sizeof(list_t));

    l->count = 0;
    l->head = NULL;
    l->tail = NULL;

    return l;
}

void list_delete(list_t *l)
{
    list_item_t *current = l->head;

    while (current)
    {
        list_item_t *next = current->next;
        free(current);
        current = next;
    }

    free(l);
}

void list_destroy(list_t *l)
{
    list_item_t *current = l->head;

    while (current)
    {
        list_item_t *next = current->next;
        free(current->value);
        free(current);
        current = next;
    }

    free(l);
}

void list_clear(list_t *list)
{
    list_item_t *current = list->head;

    while (current)
    {
        list_item_t *next = current->next;
        free(current);
        current = next;
    }

    list->count = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_push(list_t *l, void *value)
{
    list_item_t *item = MALLOC(list_item_t);

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

int list_pop(list_t *l, void **value)
{
    list_item_t *item = l->head;

    if (l->count == 0)
    {
        return 0;
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

    *(value) = item->value;
    free(item);

    return 1;
}

void list_pushback(list_t *l, void *value)
{
    list_item_t *item = MALLOC(list_item_t);

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

int list_popback(list_t *l, void **value)
{
    list_item_t *item = l->tail;

    if (l->count == 0)
    {
        return 0;
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

    *(value) = item->value;
    free(item);

    return 1;
}

int list_remove(list_t *l, void *value)
{
    FOREACH(item, l)
    {
        if (item->value == value)
        {
            if (item->prev != NULL)
            {
                item->prev->next = item->next;
            }
            else
            {
                l->head = item->next;
            }

            if (item->next != NULL)
            {
                item->next->prev = item->prev;
            }
            else
            {
                l->tail = item->prev;
            }

            l->count--;
            free(item);

            return 1;
        }
    }

    return 0;
}

int list_containe(list_t *l, void *value)
{
    FOREACH(item, l)
    {
        if (item->value == value)
        {
            return 1;
        }
    }

    return 0;
}