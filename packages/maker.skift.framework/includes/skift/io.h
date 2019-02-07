#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* io.h: filesystem file io and file operation                                */

#include <skift/generic.h>

#define FSPATH_SEP '/'
#define FSNAME_SIZE 128

#define EOF -1

typedef enum
{
    FROM_START,
    FROM_END,
    FROM_HERE
} iowhence_t;

typedef enum
{
    O_READ = FLAG(0),
    O_WRITE = FLAG(1),
    O_READWRITE = FLAG(2),
    O_CREATE = FLAG(3),
    O_APPEND = FLAG(4),
    O_TRUNC = FLAG(5),
} iooflag_t;

typedef struct
{
    int handle;
    byte *buffer;
} file_t;

typedef struct
{
    int handle;
} directory_t;

typedef enum
{
    FTYPE_REGULAR,
    FTYPE_DIRECTORY,
    FTYPE_DEVICE,
    FTYPE_PIPE,
} ftype_t;

typedef struct
{
    uint size;
    ftype_t type;
} fstat_t;

typedef struct
{
    char name[FSNAME_SIZE];
    fsnode_type_t type;
} direntry_t;

extern file_t *input_stream;
extern file_t *output_stream;
extern file_t *error_stream;

extern directory_t *working_directory;

file_t *sk_io_open(const char *path, iooflag_t oflags);
file_t *sk_io_openat(directory_t *directory, const char *path, iooflag_t oflags);
void sk_io_close(file_t *f);

uint sk_io_read(file_t *f, void *buffer, uint size);
uint sk_io_write(file_t *f, void *buffer, uint size);
uint sk_io_pread(file_t *f, void *buffer, uint size, uint offset);
uint sk_io_pwrite(file_t *f, void *buffer, uint size, uint offset);

int sk_io_seek(file_t *f, iowhence_t origin, int offset);
int sk_io_tell(file_t file, iowhence_t origin);

int sk_io_control(file_t *f, int ctl, void* arg);

// Open a directory
directory_t* sk_io_directory_open(const char *path);

// Close a directory
void sk_io_directory_close(directory_t* dir);

void sk_io_directory_read();