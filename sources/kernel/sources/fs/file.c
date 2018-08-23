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

int file_create(directory_t *relative, const char *path, filesystem_t * fs, int device, int inode)
{
    char *dir_path = malloc(strlen(path));
    char file_name[128];
    file_t *file = NULL;

    if (path_split(path, dir_path, file_name))
    {
        directory_t *parent = filesystem_get_directory(relative, path);
        file = alloc_file(file_name);
        file->parent = parent;

        sll_add((u32)file, parent->files);
        
        file->fs = fs;
        file->device = device;
        file->inode = inode;
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

    if (file != NULL && (file->fs->file_open == NULL || file->fs->file_open(file)))
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
    if (file != NULL && file->fs->file_close != NULL)
    {
        file->fs->file_close(file);
    }
}

void file_stat(file_t * file, file_stat_t * stat)
{
    if (file != NULL && file->fs->file_read !=NULL)
    {
        file->fs->file_stat(file, stat);
    }
}

int file_read(file_t *file, uint offset, void *buffer, uint n)
{
    if (file != NULL && file->fs->file_read !=NULL)
    {
        return file->fs->file_read(file, offset, buffer, n);
    }

    return 0;
}

void* file_read_all(file_t* file)
{
    void* data = NULL;

    if (file != NULL)
    {
        fstat_t stat;
        file_stat(file, &stat);

        data = malloc(stat.size);
        file_read(file, 0, data, stat.size);
    }

    return data;
}

int file_write(file_t *file, uint offset, void *buffer, uint n)
{
    if (file != NULL && file->fs->file_write !=NULL)
    {
        return file->fs->file_write(file, offset, buffer, n);
    }

    return 0;
}
