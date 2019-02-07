#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* io.h: filesystem file io and file operation                                */

#include <skift/generic.h>

#define EOF -1

typedef enum
{
    FROM_START,
    FROM_END,
    FROM_HERE
} fsseeko_t;

typedef enum
{
    O_READ = FLAG(0),
    O_WRITE = FLAG(1),
    O_READWRITE = FLAG(2),
    O_CREATE = FLAG(3),
    O_APPEND = FLAG(4),
    O_TRUNC = FLAG(5),
} fsoflags_t;

typedef struct
{
    int handle;
    byte *buffer;
} file_t;

extern file_t* stdin;
extern file_t* stdout;
extern file_t* stderr;

file_t *sk_io_open(const char *path, int oflags);

int sk_io_close(file_t *f);

uint sk_io_read(file_t *f, void *buffer, uint size);
uint sk_io_write(file_t *f, void *buffer, uint size);
uint sk_io_pread(file_t *f, void *buffer, uint size, uint offset);
uint sk_io_pwrite(file_t *f, void *buffer, uint size, uint offset);

int sk_io_seek(file_t *f, fsseeko_t origin, int offset);
int sk_io_tell(file_t file, fsseeko_t origin);