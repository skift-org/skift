/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/iostream.h>
#include <libsystem/path.h>

path_t *path(const char *raw_path)
{
    path_t *p = MALLOC(path_t);
    p->elements = list();

    const char *begin = raw_path;

    p->is_absolue = (raw_path[0] == '/');

    for (uint i = 0; i <= strlen(raw_path); i++)
    {
        if (raw_path[i] == '/' || raw_path[i] == '\0')
        {
            int lenght = (&raw_path[i] - begin) + 1;

            if (lenght > 1) // 1 for the null terminator
            {
                char *element = malloc(lenght);
                strlcpy(element, begin, lenght);
                list_pushback(p->elements, element);
            }

            // set the beginning of the next path element and skip the '/'
            begin = &raw_path[i] + 1;
        }
    }

    return p;
}

void path_delete(path_t *path)
{
    list_delete(path->elements, LIST_FREE_VALUES);
    free(path);
}

const char *path_filename(path_t *p)
{
    const char *element;

    if (list_peekback(p->elements, (void **)&element))
    {
        return element;
    }
    else
    {
        return NULL;
    }
}

const char *path_element(path_t *p, int index)
{
    const char *element;

    if (list_peekat(p->elements, index, (void **)&element))
    {
        return element;
    }
    else
    {
        return NULL;
    }
}

bool path_is_absolue(path_t *p)
{
    return p->is_absolue;
}

bool path_is_relative(path_t *p)
{
    return !p->is_absolue;
}

int path_length(path_t *p)
{
    return list_count(p->elements);
}

void path_normalize(path_t *p)
{
    list_t *stack = list();

    int depth = 0;

    list_foreach(i, p->elements)
    {
        const char *element = i->value;

        if ((strcmp(element, "..") == 0) && (depth > 0))
        {
            char *value;
            list_popback(stack, (void **)&value);
            free(value);
            depth--;
        }
        else if (strcmp(element, ".") != 0)
        {
            list_pushback(stack, strdup(element));
            depth++;
        }
    }

    list_delete(p->elements, LIST_FREE_VALUES);

    p->elements = stack;
}

void path_push(path_t *p, const char *e)
{
    int lenght = strlen(e);

    if (lenght > 0)
    {
        list_pushback(p->elements, (char *)e);
    }
}

const char *path_pop(path_t *p)
{
    const char *element = NULL;
    list_popback(p->elements, (void **)&element);
    return element;
}

path_t *path_combine(path_t *left, path_t *right)
{
    path_t *p = MALLOC(path_t);
    p->elements = list();

    // Check if the resulting path is absolue
    if (left != NULL)
    {
        p->is_absolue = left->is_absolue;
    }
    else if (right != NULL)
    {
        p->is_absolue = right->is_absolue;
    }
    else
    {
        p->is_absolue = false;
    }

    // Append the left part of the path
    if (left != NULL)
    {
        p->is_absolue = left->is_absolue;

        list_foreach(i, left->elements)
        {
            path_push(p, strdup(i->value));
        }
    }

    // Append the rigt parte of the path
    if (right != NULL)
    {
        list_foreach(i, right->elements)
        {
            path_push(p, strdup(i->value));
        }
    }

    return p;
}

path_t *path_split_at(path_t *path, int index)
{
    path_t *p = MALLOC(path_t);
    p->elements = list();
    p->is_absolue = false;

    int current_element_index = 0;
    list_foreach(i, path->elements)
    {
        if (current_element_index >= index)
        {
            path_push(p, strdup(i->value));
        }

        current_element_index++;
    }

    return p;
}

path_t *path_dup(path_t *path)
{
    path_t *p = MALLOC(path_t);
    p->elements = list();
    p->is_absolue = path->is_absolue;

    list_foreach(i, path->elements)
    {
        path_push(p, strdup(i->value));
    }

    return p;
}

void path_to_cstring(path_t *this, char *buffer, uint size)
{
    buffer[0] = '\0';

    if (path_length(this) == 0)
    {
        strlcpy(buffer, "/", size);
    }
    else
    {
        list_foreach(i, this->elements)
        {
            const char *element = (const char *)i->value;
            strnapd(buffer, '/', size);
            strncat(buffer, element, size);
        }
    }
}

void path_dump(path_t *p)
{
    for (int i = 0; i < path_length(p); i++)
    {
        printf("/%s", path_element(p, i));
    }
}