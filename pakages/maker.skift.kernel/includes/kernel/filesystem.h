#pragma once

/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/types.h>
#include <skift/utils.h>
#include <skift/list.h>
#include <skift/path.h>

#define FS_PATH_SEPARATOR '/'

struct file;
struct directory;

typedef struct
{
    int size;
    int read;
    int write;
} fstat_t;

typedef int (*file_open_t)(struct file *file);
typedef void (*file_close_t)(struct file *file);
typedef int (*file_read_t)(struct file *file, uint offset, void *buffer, uint n);
typedef int (*file_write_t)(struct file *file, uint offset, void *buffer, uint n);

typedef void (*file_stat_t)(struct file *file, fstat_t *stat);

typedef struct
{
    file_open_t  file_open;
    file_close_t file_close;
    file_read_t  file_read;
    file_write_t file_write;
    file_stat_t  file_stat;
} filesystem_t;

typedef struct
{
    char name[PATH_SIZE];
    bool is_directory;
} direntry_t;

typedef struct directory
{
    char name[PATH_FILE_NAME_SIZE];

    list_t *files;
    list_t *directories;

    struct directory *parent;
} directory_t;

typedef struct file
{
    uint device;
    uint inode;
    filesystem_t* fs;

    char name[PATH_FILE_NAME_SIZE];
    struct directory *parent;
} file_t;

void filesystem_setup();
void filesystem_dump(directory_t *relative, const char *path);

file_t *filesystem_get_file(directory_t *relative, const char *path);
directory_t *filesystem_get_directory(directory_t *relative, const char *path);

/* --- Files Operation ------------------------------------------------------ */

int file_create(directory_t *relative, const char *path, filesystem_t * fs, int device, int inode);
int file_create_device(directory_t *relative, const char *path, int flags);
int file_delete(directory_t *relative, const char *path);
int file_existe(directory_t *relative, const char *path);

int file_copy(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination);
int file_move(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination);
void file_stat(file_t *file, fstat_t *stat);

file_t *file_open(directory_t *relative, const char *path);
void file_close(file_t *file);

void* file_read_all(file_t* file);
int file_read(file_t *file, uint offset, void *buffer, uint n);
int file_write(file_t *file, uint offset, void *buffer, uint n);

/* --- Directories Operation ------------------------------------------------ */

int directory_create(directory_t *relative, const char *path, int flags);
int directory_delete(directory_t *relative, const char *path, bool recursive);
int directory_existe(directory_t *relative, const char *path);

int directory_copy(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination);
int directory_move(directory_t *relative_s, const char *source, directory_t *relative_d, const char *destination);

directory_t *directory_open(directory_t *relative, const char *path);
void directory_close(directory_t *directory);

int directory_get_files(directory_t *directory, char *name, int index);
int directory_get_directories(directory_t *directory, char *name, int index);
