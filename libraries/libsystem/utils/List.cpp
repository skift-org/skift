
#include <libsystem/utils/List.h>
#include <string.h>

List *list_create()
{
    List *list = CREATE(List);

    list->_count = 0;
    list->_head = nullptr;
    list->_tail = nullptr;

    return list;
}

void list_destroy(List *list) { list_destroy_with_callback(list, nullptr); }
void list_destroy_with_callback(List *list, ListDestroyElementCallback callback)
{
    list_clear_with_callback(list, callback);
    free(list);
}

void list_clear(List *list) { list_clear_with_callback(list, nullptr); }
void list_clear_with_callback(List *list, ListDestroyElementCallback callback)
{
    ListItem *current = list->_head;

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

    list->_count = 0;
    list->_head = nullptr;
    list->_tail = nullptr;
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
    if (list->_head == nullptr || callback(value, list->_head->value))
    {
        list_push(list, value);
    }
    else
    {
        ListItem *current = list->_head;

        while (current->next != nullptr && callback(current->next->value, value))
        {
            current = current->next;
        }

        ListItem *item = CREATE(ListItem);

        item->prev = current;
        item->next = current->next;
        item->value = value;

        if (current->next == nullptr)
        {
            list->_tail = item;
        }
        else
        {
            current->next->prev = item;
        }

        current->next = item;

        list->_count++;
    }
}

void *list_peek(List *list)
{
    if (list->_head != nullptr)
    {
        return list->_head->value;
    }
    else
    {
        return nullptr;
    }
}

bool list_peek_and_pushback(List *list, void **value)
{
    *value = list_peek(list);

    if (*value)
    {
        ListItem *item = list->_head;

        // Pop
        if (list->_count == 1)
        {
            list->_count = 0;
            list->_head = nullptr;
            list->_tail = nullptr;
        }
        else if (list->_count > 1)
        {
            item->next->prev = nullptr;
            list->_head = item->next;

            list->_count--;
        }

        // Push back
        item->prev = nullptr;
        item->next = nullptr;

        list->_count++;

        if (list->_tail == nullptr)
        {
            list->_tail = item;
            list->_head = item;
        }
        else
        {
            list->_tail->next = item;
            item->prev = list->_tail;
            list->_tail = item;
        }

        return true;
    }
    else
    {
        return false;
    }
}

void *list_peekback(List *list)
{
    if (list->_tail != nullptr)
    {
        return list->_tail->value;
    }
    else
    {
        return nullptr;
    }
}

static void list_peekat_from_head(List *list, int index, void **value)
{
    ListItem *current = list->_head;

    for (int i = 0; i < index; i++)
    {
        if (current->next)
        {
            current = current->next;
        }
    }

    *value = current->value;
}

static void list_peekat_from_back(List *list, int index, void **value)
{
    ListItem *current = list->_tail;

    for (int i = 0; i < (list->_count - index - 1); i++)
    {
        if (current->prev)
        {
            current = current->prev;
        }
    }

    *value = current->value;
}

bool list_peekat(List *list, int index, void **value)
{
    *value = nullptr;

    if (list->_count >= 1 && index >= 0 && index < list->_count)
    {
        if (index < list->_count / 2)
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

void list_insert(List *list, int index, void *value)
{
    if (list->empty())
    {
        list_pushback(list, value);
        return;
    }

    if (index == 0)
    {
        list_push(list, value);
    }

    ListItem *current = list->_head;

    for (int i = 0; i < index - 1; i++)
    {
        if (current->next)
        {
            current = current->next;
        }
    }

    ListItem *item = CREATE(ListItem);

    item->prev = current;
    item->next = current->next;
    item->value = value;

    current->next = item;

    if (list->_tail == current)
    {
        list->_tail = item;
    }

    list->_count++;
}

void list_push(List *list, void *value)
{
    ListItem *item = CREATE(ListItem);

    item->value = value;

    list->_count++;

    if (list->_head == nullptr)
    {
        list->_head = item;
        list->_tail = item;
    }
    else
    {
        list->_head->prev = item;
        item->next = list->_head;
        list->_head = item;
    }
}

bool list_pop(List *list, void **value)
{
    ListItem *item = list->_head;

    if (list->_count == 0)
    {
        return false;
    }
    else if (list->_count == 1)
    {
        list->_count = 0;
        list->_head = nullptr;
        list->_tail = nullptr;
    }
    else if (list->_count > 1)
    {
        item->next->prev = nullptr;
        list->_head = item->next;

        list->_count--;
    }

    if (value != nullptr)
    {
        *value = item->value;
    }

    return true;
}

void list_pushback(List *list, void *value)
{
    ListItem *item = CREATE(ListItem);

    item->prev = nullptr;
    item->next = nullptr;
    item->value = value;

    list->_count++;

    if (list->_tail == nullptr)
    {
        list->_tail = item;
        list->_head = item;
    }
    else
    {
        list->_tail->next = item;
        item->prev = list->_tail;
        list->_tail = item;
    }
}

void list_pushback_copy(List *list, void *value, size_t size)
{
    void *copy = malloc(size);
    memcpy(copy, value, size);
    list_pushback(list, copy);
}

bool list_popback(List *list, void **value)
{
    ListItem *item = list->_tail;

    if (list->_count == 0)
    {
        return false;
    }
    else if (list->_count == 1)
    {
        list->_count = 0;
        list->_head = nullptr;
        list->_tail = nullptr;
    }
    else if (list->_count > 1)
    {
        item->prev->next = nullptr;
        list->_tail = item->prev;

        list->_count--;
    }

    if (value != nullptr)
    {
        *value = item->value;
    }

    return true;
}

bool list_remove(List *list, void *value) { return list_remove_with_callback(list, value, nullptr); }
bool list_remove_with_callback(List *list, void *value, ListDestroyElementCallback callback)
{
    for (ListItem *item = list->_head; item != nullptr; item = item->next)
    {
        if (item->value == value)
        {
            if (item->prev != nullptr)
            {
                item->prev->next = item->next;
            }
            else
            {
                list->_head = item->next;
            }

            if (item->next != nullptr)
            {
                item->next->prev = item->prev;
            }
            else
            {
                list->_tail = item->prev;
            }

            list->_count--;

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

bool list_remove_at(List *list, int index)
{
    return list_remove_at_with_callback(list, index, nullptr);
}

bool list_remove_at_with_callback(List *list, int index, ListDestroyElementCallback callback)
{
    void *value = nullptr;
    list_peekat(list, index, &value);
    return list_remove_with_callback(list, value, callback);
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

bool list_iterate(List *list, void *target, ListIterationCallback callback)
{
    ListItem *current = list->_head;

    while (current)
    {
        ListItem *next = current->next;
        if (callback(target, current->value) == Iteration::STOP)
        {
            return false;
        }
        current = next;
    }

    return true;
}
