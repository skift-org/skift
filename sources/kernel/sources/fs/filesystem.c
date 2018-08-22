// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include <string.h>
#include "kernel/filesystem.h"
#include "kernel/logging.h"

directory_t *root = NULL;

directory_t *alloc_directorie(const char *name);

void filesystem_setup()
{
    info("Allocating root directorie.");
    root = alloc_directorie("ROOT");
}

directory_t *filesystem_get_directory(directory_t *relative, const char *path)
{
    char buffer[128];
    
    directory_t * current = relative ? relative : root;

    for (int i = 0; path_read(path, i, buffer); i++) 
    {
        if (strcmp(buffer, "..") == 0)
        {
            current = current->parent;
        }
        else if (strcmp(buffer, ".") == 0)
        {
            current = current;
        }
        else
        {
            SLL_FOREARCH(i, current->directories)
            {
                directory_t *d = (directory_t *)i->data;
                if (strcmp(buffer, d->name) == 0)
                    current = d;
            }
        }
    }
    
    return current;
}

file_t *filesystem_get_file(directory_t *relative, const char *path)
{
    char *dir_name = malloc(strlen(path));
    char file_name[128];
    file_t *file = NULL;

    if (path_split(path, dir_name, file_name))
    {
        directory_t *dir = filesystem_get_directory(relative, dir_name);

        SLL_FOREARCH(i, dir->files)
        {
            file_t* f = (file_t *)i->data;
            if (strcmp(file_name, f->name) == 0)
                file = f;
        }
    }

    free(dir_name);
    return file;
}

/* --- Dump ---------------------------------------------------------------- */

void dump_directorie(directory_t *current, int depth, char * buffer)
{
   
    for (int i = 0; i < depth; i++)
    {
        printf("\t");
    }

    printf("&f%s/\n&7", current->name);

    for (size_t i = 0; directory_get_directories(current, buffer, i); i++)
    {
        dump_directorie(filesystem_get_directory(current, buffer), depth + 1, buffer);
    }

    for (size_t i = 0; (directory_get_files(current, buffer, i)); i++)
    {
        for (int y = 0; y < depth + 1; y++)
        {
            printf("\t");
        }

        printf("&7%s\n", buffer);
    }
}

void filesystem_dump(directory_t *relative, const char *path)
{
    char * buffer = malloc(PATH_FILE_NAME_SIZE);
    dump_directorie(filesystem_get_directory(relative, path), 0, buffer);
    free(buffer);
}

