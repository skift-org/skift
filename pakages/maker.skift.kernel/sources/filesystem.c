/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <string.h>
#include "kernel/logger.h"
#include "kernel/filesystem.h"

directory_t *root = NULL;

directory_t *alloc_directorie(const char *name);

void filesystem_setup()
{
    log("Allocating root directorie...");
    root = alloc_directorie("ROOT");
}

directory_t *filesystem_get_directory(directory_t *relative, const char *path)
{
    char buffer[128];

    directory_t *current = relative ? relative : root;

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
            file_t *f = (file_t *)i->data;
            if (strcmp(file_name, f->name) == 0)
                file = f;
        }
    }

    free(dir_name);
    return file;
}

/* --- Dump ---------------------------------------------------------------- */

void dump_directorie(directory_t *current, int depth, char *buffer)
{

    for (int i = 0; i < depth; i++)
    {
        printf("\t");
    }

    printf("%s/\n", current->name);

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

        printf("%s\n", buffer);
    }
}

void filesystem_dump(directory_t *relative, const char *path)
{
    char *buffer = malloc(PATH_FILE_NAME_SIZE);
    dump_directorie(filesystem_get_directory(relative, path), 0, buffer);
    free(buffer);
}

/* === Directories ========================================================== */

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

/* === FILES ================================================================ */

file_t *alloc_file(const char *name)
{
    file_t *file = MALLOC(file_t);

    strncpy((char *)&file->name, name, PATH_FILE_NAME_SIZE);

    return file;
}

/* --- Create/Delete/Existe ------------------------------------------------- */

int file_create(directory_t *relative, const char *path, filesystem_t *fs, int device, int inode)
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
    file_t *file = filesystem_get_file(relative, path);

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

void file_stat(file_t *file, fstat_t *stat)
{
    if (file != NULL && file->fs->file_stat != NULL)
    {
        file->fs->file_stat(file, stat);
    }
}

int file_read(file_t *file, uint offset, void *buffer, uint n)
{
    if (file != NULL && file->fs->file_read != NULL)
    {
        return file->fs->file_read(file, offset, buffer, n);
    }

    return 0;
}

void *file_read_all(file_t *file)
{
    void *data = NULL;

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
    if (file != NULL && file->fs->file_write != NULL)
    {
        return file->fs->file_write(file, offset, buffer, n);
    }

    return 0;
}
