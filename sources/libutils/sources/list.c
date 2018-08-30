#include <stdlib.h>
#include <stdio.h>

#include "ds/list.h"

list_item_t *list_item_alloc(void* value)
{
    list_item_t *item = malloc(sizeof(list_item_t));

    item->prev = NULL;
    item->next = NULL;
    item->value = value;

    return item;
}

list_t *list_alloc()
{
    list_t *list = malloc(sizeof(list_t));

    list->count = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

void list_free(list_t *list)
{
    list_item_t *current = list->head;

    while (current)
    {
        list_item_t *next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

void list_destrory(list_t *list)
{
    list_item_t *current = list->head;

    while (current)
    {
        list_item_t *next = current->next;
        free((void *)current->value);
        free(current);
        current = next;
    }

    free(list);
}

void list_print(list_t *list)
{
    printf("c:%d ", list->count);
    if (list->head) printf("h:%d", list->head->value);
    else printf("h:NULL ");

    if (list->tail) printf("t:%d",list->tail->value);
    else printf("t:NULL ");

    printf("\n");

    FOREACH(item, list)
    {
        if (item->prev) printf("%d", item->prev->value);
        else printf("-");
        printf(" %d ", item->value);
        if (item->next) printf("%d", item->next->value);
        else printf("-");
        printf("\n");
    }
    printf("\n");
}

void list_push(list_t *list, void* value)
{
    list_item_t *item = list_item_alloc(value);
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

int list_pop(list_t *list, void **value)
{
    list_item_t *item = list->head;

    if (list->count == 0)
    {
        return 0;
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

    *value = item->value;
    free(item);

    return 1;
}

void list_pushback(list_t *list, void* value)
{
    list_item_t *item = list_item_alloc(value);
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

int list_popback(list_t *list, void **value)
{
    list_item_t *item = list->tail;

    if (list->count == 0)
    {
        return 0;
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

    *value = item->value;
    free(item);

    return 1;
}

int list_remove(list_t *list, void* value)
{
    FOREACH(item, list)
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
            free(item);

            return 1;
        }
    }

    return 0;
}

int list_containe(list_t *list, void* value)
{
    FOREACH(item, list)
    {
        if (item->value == value)
        {
            return 1;
        }
    }

    return 0;
}