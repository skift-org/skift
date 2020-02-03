/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/path.h>

Path *path(const char *raw_path)
{
    Path *p = __create(Path);
    p->elements = list_create();

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

void path_delete(Path *path)
{
    list_destroy(path->elements, LIST_FREE_VALUES);
    free(path);
}

const char *path_filename(Path *p)
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

const char *path_element(Path *p, int index)
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

bool path_is_absolue(Path *p)
{
    return p->is_absolue;
}

bool path_is_relative(Path *p)
{
    return !p->is_absolue;
}

int path_length(Path *p)
{
    return list_count(p->elements);
}

void path_normalize(Path *p)
{
    List *stack = list_create();

    int depth = 0;

    list_foreach(const char, element, p->elements)
    {
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

    list_destroy(p->elements, LIST_FREE_VALUES);

    p->elements = stack;
}

void path_push(Path *p, const char *e)
{
    int lenght = strlen(e);

    if (lenght > 0)
    {
        list_pushback(p->elements, (char *)e);
    }
}

const char *path_pop(Path *p)
{
    const char *element = NULL;
    list_popback(p->elements, (void **)&element);
    return element;
}

Path *path_combine(Path *left, Path *right)
{
    Path *p = __create(Path);
    p->elements = list_create();

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

        list_foreach(const char, i, left->elements)
        {
            path_push(p, strdup(i));
        }
    }

    // Append the rigt parte of the path
    if (right != NULL)
    {
        list_foreach(const char, i, right->elements)
        {
            path_push(p, strdup(i));
        }
    }

    return p;
}

Path *path_split_at(Path *path, int index)
{
    Path *p = __create(Path);
    p->elements = list_create();
    p->is_absolue = false;

    int current_element_index = 0;

    list_foreach(const char, i, path->elements)
    {
        if (current_element_index >= index)
        {
            path_push(p, strdup(i));
        }

        current_element_index++;
    }

    return p;
}

Path *path_dup(Path *path)
{
    Path *p = __create(Path);
    p->elements = list_create();
    p->is_absolue = path->is_absolue;

    list_foreach(const char, i, path->elements)
    {
        path_push(p, strdup(i));
    }

    return p;
}

void Patho_cstring(Path *this, char *buffer, uint size)
{
    buffer[0] = '\0';

    if (path_length(this) == 0)
    {
        strlcpy(buffer, "/", size);
    }
    else
    {
        list_foreach(const char, i, this->elements)
        {
            const char *element = (const char *)i;
            strnapd(buffer, '/', size);
            strncat(buffer, element, size);
        }
    }
}

void path_dump(Path *p)
{
    for (int i = 0; i < path_length(p); i++)
    {
        printf("/%s", path_element(p, i));
    }
}