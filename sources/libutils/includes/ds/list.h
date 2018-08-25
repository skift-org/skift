#pragma once
#define FOREACH(item, list) for (list_item_t *item = list->head; item != NULL; item = item->next)

typedef struct list_node
{
    struct list_node *prev;
    struct list_node *next;
    int value;
} list_item_t;

typedef struct list
{
    int count;
    list_item_t *head;
    list_item_t *tail;
} list_t;

list_t *list_alloc();
void list_free(list_t *list);
void list_destrory(list_t *list);

void list_print(list_t *list);

void list_push(list_t *list, int value);
int list_pop(list_t *list, int *value);

void list_pushback(list_t *list, int value);
int list_popback(list_t *list, int *value);

/** Remove the first occurence of a value in the list.*/
int list_remove(list_t *list, int value);
int list_containe(list_t* list, int value);

