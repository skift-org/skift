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

    for(int i = 0; raw_path[i]; i++)
    {
        char c = raw_path[i];

        if (c == '/')
        {
            int elem_size = (&raw_path[i] - begin) + 1;

            if (elem_size > 1)
            {
                char* element = malloc(elem_size);
                memcpy(element, begin, elem_size);
                list_pushback(p->elements, element);
            }
        }
    }
    
    return p;
}

void path_delete(path_t* path)
{
    list_delete(path, LIST_FREE_VALUES);
    free(path);
}





/*
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    const char* path = "/home//nicolas/project/skift";

    if (path[0] == '/') 
    {
        printf("This a absolute path\n");
    }

    const char* begin = path;

    for(int i = 0; i <= strlen(path); i++)
    {
        if (path[i] == '/' || path[i] == '\0')
        {
            int lenght = &path[i] - begin;
            if (lenght > 0)
            {
                char buffer[lenght + 1];
                buffer[lenght] = '\0';
                strncpy(buffer, begin, lenght);
                printf("%d '%s'\n", lenght, buffer);
            }
            begin = &path[i] + 1;
        }
    }
    
    return 0;
}
*/

















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