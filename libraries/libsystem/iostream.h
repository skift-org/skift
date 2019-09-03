#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* iostream.h: generic io interface                                           */

#include <libsystem/runtime.h>
#include <libsystem/path.h>

struct s_iostream;

typedef enum
{
    IOSTREAM_WHENCE_START,
    IOSTREAM_WHENCE_HERE,
    IOSTREAM_WHENCE_END,
} iostream_whence_t;

typedef enum
{
    IOSTREAM_TYPE_UNKNOWN,

    IOSTREAM_TYPE_REGULAR,
    IOSTREAM_TYPE_DEVICE,
    IOSTREAM_TYPE_DIRECTORY,
    IOSTREAM_TYPE_FIFO,
} iostream_type_t;

typedef enum
{
    IOSTREAM_READ = FLAG(0),
    IOSTREAM_WRITE = FLAG(1),

    IOSTREAM_CREATE = FLAG(2),
    IOSTREAM_APPEND = FLAG(3),
    IOSTREAM_TRUNC = FLAG(4),

    IOSTREAM_BUFFERED_READ = FLAG(5),
    IOSTREAM_BUFFERED_WRITE = FLAG(6)
} iostream_flag_t;

typedef enum
{
    IOSTREAM_BUFFERED_LINE,
    IOSTREAM_BUFFERED_BLOCK,
    IOSTREAM_BUFFERED_NONE,
} iostream_buffer_mode_t;

typedef struct
{
    uint size;
    iostream_type_t type;
} iostream_stat_t;

typedef struct
{
    char name[PATH_ELEMENT_LENGHT];
    iostream_stat_t stat;
} iostream_direntry_t;

typedef int iostream_operation_read_t(struct s_iostream *stream, void *buffer, uint size);
typedef int iostream_operation_write_t(struct s_iostream *stream, const void *buffer, uint size);
typedef int iostream_operation_tell_t(struct s_iostream *stream, iostream_whence_t whence);
typedef int iostream_operation_seek_t(struct s_iostream *stream, int offset, iostream_whence_t whence);
typedef int iostream_operation_stat_t(struct s_iostream *stream, iostream_stat_t *stat);
typedef int iostream_operation_call_t(struct s_iostream *stream, int request, void *arg);
typedef int iostream_operation_close_t(struct s_iostream *stream);

#define IOSTREAM_BUFFER_SIZE 512

typedef struct s_iostream
{
    iostream_flag_t flags;

    iostream_operation_read_t *read;
    iostream_operation_write_t *write;
    iostream_operation_tell_t *tell;
    iostream_operation_seek_t *seek;
    iostream_operation_stat_t *stat;
    iostream_operation_call_t *call;
    iostream_operation_close_t *close;

    iostream_buffer_mode_t read_mode;
    void *write_buffer;
    int write_used;

    iostream_buffer_mode_t write_mode;
    void *read_buffer;
    int read_used;
    int read_head;

    bool has_unget;
    int unget_char;

    int fd;
    void *p;
} iostream_t;

extern iostream_t *in_stream;
extern iostream_t *out_stream;
extern iostream_t *err_stream;
extern iostream_t *log_stream;

iostream_t *iostream(iostream_flag_t flags);

void iostream_delete(iostream_t *stream);

void iostream_set_read_buffer_mode(iostream_t *this, iostream_buffer_mode_t mode);

void iostream_set_write_buffer_mode(iostream_t *this, iostream_buffer_mode_t mode);

iostream_t *iostream_open(const char *path, iostream_flag_t flags);

void iostream_close(iostream_t *stream);

int iostream_read(iostream_t *stream, void *buffer, uint size);

int iostream_write(iostream_t *stream, const void *buffer, uint size);

int iostream_send(iostream_t* stream);

int iostream_recv(iostream_t* stream);

int iostream_flush(iostream_t *stream);

int iostream_call(iostream_t *stream, int request, void *arg);

int iostream_seek(iostream_t *stream, int offset, iostream_whence_t whence);

int iostream_tell(iostream_t *stream, iostream_whence_t whence);

int iostream_stat(iostream_t *stream, iostream_stat_t *stat);

int iostream_puts(iostream_t *stream, const char *string);

#define puts(__string) iostream_puts(out_stream, __string)

char *iostream_gets(iostream_t *stream, char *string, int n);

int iostream_putchar(iostream_t *stream, int c);

int iostream_getchar(iostream_t *stream);

int iostream_ungetchar(iostream_t *stream, char c);

int iostream_printf(iostream_t *stream, const char *fmt, ...);

#define printf(__args...) iostream_printf(out_stream, __args)

int iostream_vprintf(iostream_t *stream, const char *fmt, va_list va);

#define vprintf(__fmt, __va) iostream_vprintf(out_stream, __fmt, __va)

int iostream_scanf(iostream_t *stream, const char *fmt, ...);

int iostream_vscanf(iostream_t *stream, const char *fmt, va_list va);