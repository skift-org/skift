// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include <string.h>
#include <stdlib.h>
#include "kernel/filesystem.h"

directory_t *alloc_directorie(const char *name)
{
    directory_t *dir = MALLOC(directory_t);

    dir->name[0] = '\0';
    strncpy((char *)&dir->name, name, PATH_FILE_NAME_SIZE);
    dir->directories = sll_new();
    dir->files = sll_new();

    return dir;
}

/* --- Create/Delete/Existe ------------------------------------------------- */

int directory_create(directory_t *relative, const char *path, int flags)
{
    UNUSED(flags);

    char *dir_path = malloc(strlen(path));
    char dir_name[128];
    directory_t *dir = NULL;

    if (path_split(path, dir_path, dir_name))
    {
        directory_t *parent = filesystem_get_directory(relative, dir_path);
        dir = alloc_directorie(dir_name);
        dir->parent = parent;
        sll_add((u32)dir, parent->directories);
    }

    free(dir_path);
    return dir == NULL ? 0 : 1;
}

int directory_delete(directory_t *relative, const char *path, bool recursive)
{
    STUB(relative, path, recursive);
    return 1;
}

int directory_existe(directory_t *relative, const char *path)
{
    if (filesystem_get_directory(relative, path) != NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* --- Move/Copy ------------------------------------------------------------ */

int directory_copy(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination)
{
    STUB(relative_s, source, relative_d, destination);
    return 1;
}

int directory_move(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination)
{
    STUB(relative_s, source, relative_d, destination);
    return 1;
}

/* --- Open/Close/Read/Write ------------------------------------------------ */

directory_t *directory_open(directory_t *relative, const char *path)
{
    return filesystem_get_directory(relative, path);
}

void directory_close(directory_t *directory)
{
    UNUSED(directory);
}

int directory_get_files(directory_t *directory, char *name, int index)
{
    name[0] = '\0';

    SLL_FOREARCH(i, directory->files)
    {
        if (index == 0)
        {
            strcpy(name, ((file_t *)i->data)->name);
            return 1;
        }

        index--;
    }

    return 0;
}

int directory_get_directories(directory_t *directory, char *name, int index)
{
    name[0] = '\0';

    SLL_FOREARCH(i, directory->directories)
    {
        if (index == 0)
        {
            strcpy(name, ((directory_t *)i->data)->name);
            return 1;
        }

        index--;
    }

    return 0;
}
