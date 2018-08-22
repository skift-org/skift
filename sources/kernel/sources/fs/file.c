// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include <string.h>
#include "kernel/filesystem.h"

file_t *alloc_file(const char *name)
{
    file_t *file = MALLOC(file_t);

    strncpy((char *)&file->name, name, PATH_FILE_NAME_SIZE);

    return file;
}

/* --- Create/Delete/Existe ------------------------------------------------- */

int file_create(directory_t *relative, const char *path, int flags)
{
    UNUSED(flags);

    char *dir_path = malloc(strlen(path));
    char file_name[128];
    file_t *file = NULL;

    if (path_split(path, dir_path, file_name))
    {
        directory_t *parent = filesystem_get_directory(relative, path);
        file = alloc_file(file_name);
        file->parent = parent;
        sll_add((u32)file, parent->files);
    }

    free(dir_path);
    return file == NULL ? 0 : 1;
}

int file_delete(directory_t *relative, const char *path)
{
    STUB(relative, path);
    return 1;
}

int file_existe(directory_t *relative, const char *path)
{
    if (filesystem_get_file(relative, path) != NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* --- Move/Copy ------------------------------------------------------------ */

int file_copy(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination)
{
    STUB(relative_s, source, relative_d, destination);
    return 1;
}

int file_move(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination)
{
    STUB(relative_s, source, relative_d, destination);
    return 1;
}

/* --- Open/Close/Read/Write ------------------------------------------------ */

file_t *file_open(directory_t *relative, const char *path)
{
    file_t * file = filesystem_get_file(relative, path);

    if (file != NULL && (file->open == NULL || file->open(file)))
    {
        return file;
    }
    else
    {
        return NULL;
    }
}

void file_close(file_t *file)
{
    if (file != NULL && file->close != NULL)
    {
        file->close(file);
    }
}

int file_read(file_t *file, uint offset, void *buffer, uint n)
{
    if (file != NULL && file->read !=NULL)
    {
        return file->read(file, offset, buffer, n);
    }

    return 0;
}

int file_write(file_t *file, uint offset, void *buffer, uint n)
{
    if (file != NULL && file->write !=NULL)
    {
        return file->write(file, offset, buffer, n);
    }

    return 0;
}
