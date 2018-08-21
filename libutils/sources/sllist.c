// Single Linked sllist_t
#include "ds/sllist.h"

sllist_node_t *createnode(int data)
{
    sllist_node_t *newsllist_node_t = malloc(sizeof(sllist_node_t));

    newsllist_node_t->data = data;
    newsllist_node_t->next = NULL;

    return newsllist_node_t;
}

sllist_t *sll_new()
{
    sllist_t *list = malloc(sizeof(sllist_t));
    list->head = NULL;

    return list;
}

void sll_display(sllist_t *list)
{
    sllist_node_t *current = list->head;

    if (list->head == NULL)
        return;

    while (current->next != NULL)
    {
        printf("%d,", current->data);
        current = current->next;
    }

    printf("%d\n", current->data);
}

void sll_add(int data, sllist_t *list)
{
    sllist_node_t *current = NULL;

    if (list->head == NULL)
    {
        list->head = createnode(data);
        list->count++;
    }
    else
    {
        current = list->head;

        while (current->next != NULL)
        {
            current = current->next;
        }

        current->next = createnode(data);
        list->count++;
    }
}

void sll_delete(int data, sllist_t *list)
{
    sllist_node_t *current = list->head;
    sllist_node_t *previous = current;
    while (current != NULL)
    {
        if (current->data == data)
        {
            previous->next = current->next;
            if (current == list->head)
                list->head = current->next;
            free(current);
            list->count--;
            return;
        }
        previous = current;
        current = current->next;
    }
}

sllist_node_t* sll_index(sllist_t *list, uint index)
{
    sllist_node_t* node = list->head;

    for(size_t i = 0; i < index - 1; i++)
    {
        if (node != NULL)
        {
            node = node->next;
        }
        else
        {
            return NULL;
        }
    }

    return node;
}

void sll_reverse(sllist_t *list)
{
    sllist_node_t *reversed = NULL;
    sllist_node_t *current = list->head;
    sllist_node_t *temp = NULL;
    while (current != NULL)
    {
        temp = current;
        current = current->next;
        temp->next = reversed;
        reversed = temp;
    }
    list->head = reversed;
}

void sll_free(sllist_t *list)
{
    sllist_node_t *current = list->head;
    sllist_node_t *next = current;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

void sll_destroy(sllist_t *list)
{
    sllist_node_t *current = list->head;
    sllist_node_t *next = current;
    while (current != NULL)
    {
        next = current->next;
        free((void *)current->data);
        free(current);
        current = next;
    }
    free(list);
}