#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* iostream.h: generic io interface                                           */

#include <skift/generic.h>

struct s_iostream;

typedef enum
{
    IOSTREAM_BEGINNIN,
    IOSTREAM_END,
} iostream_whence_t;

typedef enum
{
    IOSTREAM_CREATE,
    IOSTREAM_TRUNC,
} iostream_flag_t;

typedef struct
{
    int size;
} iostream_stat_t;

typedef int iostream_read_t(struct s_iostream *stream, void *buffer, uint size);
typedef int iostream_write_t(struct s_iostream *stream, void *buffer, uint size);
typedef int iostream_tell_t(struct s_iostream *stream, iostream_whence_t whence);
typedef int iostream_seek_t(struct s_iostream *stream, int offset, iostream_whence_t whence);
typedef int iostream_fstat_t(struct s_iostream *stream, iostream_stat_t* stat);
typedef int iostream_ioctl_t(struct s_iostream *stream, int request, void *arg);
typedef int iostream_close_t(struct s_iostream *stream);

typedef struct s_iostream
{
    iostream_read_t *read;
    iostream_write_t *write;
    iostream_tell_t *tell;
    iostream_seek_t *seek;
    iostream_fstat_t *fstat;
    iostream_ioctl_t *ioctl;
    iostream_close_t *close;

#define IOSTREAM_BUFFER_SIZE 512

    void* write_buffer;
    uint write_head;

    void *p;
} iostream_t;

iostream_t *iostream(
    iostream_read_t *read,
    iostream_write_t *write,
    iostream_tell_t *tell,
    iostream_seek_t *seek,
    iostream_fstat_t *fstat,
    iostream_ioctl_t *ioctl,
    iostream_close_t *close,

    bool write_buffered,
    bool read_buffered,

    void *p);

void iostream_delete(iostream_t *stream);

int iostream_read(iostream_t *stream, void *buffer, uint size);
int iostream_write(iostream_t *stream, void *buffer, uint size);
int iostream_flush(iostream_t *stream);
int iostream_tell(iostream_t *stream, iostream_whence_t whence);
int iostream_seek(iostream_t *stream, int offset, iostream_whence_t whence);
int iostream_fstat(iostream_t *stream, iostream_stat_t* stat);
int iostream_ioctl(iostream_t *stream, int request, void *arg);

int iostream_printf(iostream_t *stream, const char *fmt, ...);
int iostream_scanf(iostream_t *stream, const char *fmt, ...);