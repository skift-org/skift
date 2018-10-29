#include <stdlib.h>
#include <stdio.h>

#include <skift/list.h>

list_item_t *list_item(void* value)
{
    list_item_t *item = MALLOC(list_item_t);

    item->prev = NULL;
    item->next = NULL;
    item->value = value;

    return item;
}

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
        free((void *)current->value);
        free(current);
        current = next;
    }

    free(l);
}

void list_print(list_t *l)
{
    printf("c:%d ", l->count);
    if (l->head) printf("h:%d", l->head->value);
    else printf("h:NULL ");

    if (l->tail) printf("t:%d",l->tail->value);
    else printf("t:NULL ");

    printf("\n");

    FOREACH(item, l)
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

void list_push(list_t *l, void* value)
{
    list_item_t *item = list_item(value);
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

void list_pushback(list_t *l, void* value)
{
    list_item_t *item = list_item(value);
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

int list_remove(list_t *l, void* value)
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

int list_containe(list_t *l, void* value)
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