/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/path.h>

Path *path_create(const char *raw_path)
{
    Path *path = __create(Path);
    path->elements = list_create();

    const char *begin = raw_path;

    path->is_absolue = (raw_path[0] == PATH_SEPARATOR);

    for (size_t i = 0; i <= strlen(raw_path); i++)
    {
        if (raw_path[i] == PATH_SEPARATOR || raw_path[i] == '\0')
        {
            int lenght = (&raw_path[i] - begin) + 1;

            if (lenght > 1) // 1 for the null terminator
            {
                char *element = malloc(lenght);
                strlcpy(element, begin, lenght);
                list_pushback(path->elements, element);
            }

            // set the beginning of the next path element and skip the '/'
            begin = &raw_path[i] + 1;
        }
    }

    return path;
}

void path_destroy(Path *path)
{
    list_destroy_with_callback(path->elements, (ListDestroyElementCallback)free);
    free(path);
}

const char *path_filename(Path *path)
{
    const char *element;

    if (list_peekback(path->elements, (void **)&element))
    {
        return element;
    }
    else
    {
        return NULL;
    }
}

const char *path_peek_at(Path *path, int index)
{
    const char *element;

    if (list_peekat(path->elements, index, (void **)&element))
    {
        return element;
    }
    else
    {
        return NULL;
    }
}

bool path_is_absolue(Path *path)
{
    return path->is_absolue;
}

bool path_is_relative(Path *path)
{
    return !path->is_absolue;
}

size_t path_element_count(Path *path)
{
    return list_count(path->elements);
}

void path_normalize(Path *path)
{
    List *stack = list_create();

    int depth = 0;

    list_foreach(const char, element, path->elements)
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

    list_destroy_with_callback(path->elements, (ListDestroyElementCallback)free);
    path->elements = stack;
}

void path_push(Path *path, const char *element)
{
    int lenght = strlen(element);

    if (lenght > 0)
    {
        list_pushback(path->elements, (char *)element);
    }
}

const char *path_pop(Path *path)
{
    const char *element = NULL;
    list_popback(path->elements, (void **)&element);
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

Path *path_clone(Path *path)
{
    Path *clone = __create(Path);
    clone->elements = list_create();
    clone->is_absolue = path->is_absolue;

    list_foreach(const char, i, path->elements)
    {
        path_push(clone, strdup(i));
    }

    return clone;
}

void path_to_cstring(Path *this, char *buffer, uint size)
{
    buffer[0] = '\0';

    if (path_element_count(this) == 0)
    {
        strnapd(buffer, PATH_SEPARATOR, size);
    }
    else
    {
        list_foreach(const char, i, this->elements)
        {
            const char *element = (const char *)i;
            strnapd(buffer, PATH_SEPARATOR, size);
            strncat(buffer, element, size);
        }
    }
}

void path_dump(Path *p)
{
    for (size_t i = 0; i < path_element_count(p); i++)
    {
        printf("/%s", path_peek_at(p, i));
    }
}