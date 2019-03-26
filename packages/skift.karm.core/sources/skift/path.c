/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include <skift/path.h>

path_t* path(const char* raw_path)
{
    path_t* p = MALLOC(path_t);
    p->elements = list();

    const char* begin = raw_path;

    p->is_absolue = (raw_path[0] == '/');
    
    for(int i = 0; raw_path[i]; i++)
    {
        if (raw_path[i] == '/' || raw_path[i] == '\0')
        {
            int lenght = &raw_path[i] - begin;

            if(lenght > 0)
            {
                char* element = malloc(lenght + 1);
                element[lenght] = '\0';
                strncpy(element, begin, lenght);
                list_pushback(p->elements, element);
            }

            // set the beginning of the next path element and skip the '/'
            begin = &raw_path[i] + 1;
        }
    }
    
    return p;
}

void path_delete(path_t* path)
{
    list_delete(path->elements, LIST_FREE_VALUES);
    free(path);
}

const char* path_filename(path_t* p)
{
    const char* element;

    if (list_peekback(p->elements, (void**)&element))
    {
        return element;
    }

    return NULL;
}

bool path_is_absolue(path_t* p)
{
    return p->is_absolue;
}

bool path_is_relative(path_t* p)
{
    return !p->is_absolue;
}

int path_length(path_t* p)
{
    return list_count(p->elements);
}

void path_normalize(path_t* p)
{
    list_t* stack = list();

    int depth = 0;

    FOREACH(i, p->elements)
    {
        const char* element = i->value;

        if ((strcmp(element, "..") == 0) && (depth > 0))
        {
            char* value;
            list_pop(stack, (void**)&value);
            free(value);
            depth--;
        }
        else if (strcmp(element, ".") != 0)
        {
            list_push(stack, strdup(element));
            depth++;
        }
    }

    list_delete(p->elements, LIST_FREE_VALUES);

    p->elements = stack;
}

void path_append(path_t* p, const char* e)
{
    int lenght = strlen(e);

    if(lenght > 0)
    {
        list_pushback(p->elements, strdup(e));
    }
}

path_t* path_combine(path_t* left, path_t* right)
{
    path_t* p = MALLOC(path_t);
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

        FOREACH(i, left->elements)
        {
            path_append(p, i->value);
        }
    }

    // Append the rigt parte of the path
    if (right != NULL)
    {
        FOREACH(i, right->elements)
        {
            path_append(p, i->value);
        }
    }

    return p;
}

/* --- Old path api --------------------------------------------------------- */

int path_read(const char *path, int index, char *buffer)
{
    int current_index = 0;
    buffer[0] = '\0';

    if (path[0] == '/')
        path++;

    for (int i = 0; path[i]; i++)
    {
        char c = path[i];

        if (current_index == index)
        {
            if (c == '/')
            {
                break;
            }
            else
            {
                strapd(buffer, c);
            }
        }

        if (c == '/')
        {
            current_index++;
            buffer[0] = '\0';
        }
    }

    return (current_index == index && strlen(buffer)) ? 1 : 0;
}

int path_len(const char *path)
{
    char buffer[128];
    
    int count = 0;
    while(path_read(path, count, buffer)) count++;

    return count;
}

int path_split(const char *path, char *dir, char *file)
{
    if (path == NULL || dir == NULL || file == NULL)
        return 0;

    int pathlen = path_len(path);

    dir[0] = '\0';
    file[0] = '\0';

    char buffer[128];

    for (int i = 0; path_read(path, i, buffer); i++)
    {
        if (i < pathlen - 1)
        {
            strcat(dir, buffer);
            strapd(dir, '/');
        }
        else
        {
            strcpy(file, buffer);
        }
    }

    return 1;
}