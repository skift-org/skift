#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* iostream.h: generic io interface                                           */

#include <libsystem/runtime.h>
#include <libsystem/path.h>

struct IOStream;

typedef enum
{
    IOSTREAM_WHENCE_START,
    IOSTREAM_WHENCE_HERE,
    IOSTREAM_WHENCE_END,
} IOStreamWhence;

typedef enum
{
    IOSTREAM_TYPE_UNKNOWN,

    IOSTREAM_TYPE_REGULAR,
    IOSTREAM_TYPE_DEVICE,
    IOSTREAM_TYPE_DIRECTORY,
    IOSTREAM_TYPE_FIFO,
} IOStreamType;

typedef enum
{
    IOSTREAM_READ = FLAG(0),
    IOSTREAM_WRITE = FLAG(1),

    IOSTREAM_CREATE = FLAG(2),
    IOSTREAM_APPEND = FLAG(3),
    IOSTREAM_TRUNC = FLAG(4),

    IOSTREAM_BUFFERED_READ = FLAG(5),
    IOSTREAM_BUFFERED_WRITE = FLAG(6)
} IOStreamFlag;

typedef enum
{
    IOSTREAM_BUFFERED_LINE,
    IOSTREAM_BUFFERED_BLOCK,
    IOSTREAM_BUFFERED_NONE,
} IOStreamBufferMode;

typedef struct
{
    uint size;
    IOStreamType type;
} IOStreamState;

typedef struct
{
    char name[PATH_ELEMENT_LENGHT];
    IOStreamState stat;
} IOStreamDirentry;

typedef int IOStreamRead(struct IOStream *stream, void *buffer, uint size);
typedef int IOStreamWrite(struct IOStream *stream, const void *buffer, uint size);
typedef int IOStreamTell(struct IOStream *stream, IOStreamWhence whence);
typedef int IOStreamSeek(struct IOStream *stream, int offset, IOStreamWhence whence);
typedef int IOStreamStat(struct IOStream *stream, IOStreamState *stat);
typedef int IOStreamCall(struct IOStream *stream, int request, void *arg);
typedef int IOStreamClose(struct IOStream *stream);

#define IOSTREAM_BUFFER_SIZE 512

typedef struct IOStream
{
    IOStreamFlag flags;

    IOStreamRead *read;
    IOStreamWrite *write;
    IOStreamTell *tell;
    IOStreamSeek *seek;
    IOStreamStat *stat;
    IOStreamCall *call;
    IOStreamClose *close;

    IOStreamBufferMode read_mode;
    void *write_buffer;
    int write_used;

    IOStreamBufferMode write_mode;
    void *read_buffer;
    int read_used;
    int read_head;

    bool has_unget;
    int unget_char;

    int fd;
    void *p;
} IOStream;

extern IOStream *in_stream;
extern IOStream *out_stream;
extern IOStream *err_stream;
extern IOStream *log_stream;

IOStream *iostream_create(IOStreamFlag flags);

void iostream_destroy(IOStream *stream);

void iostream_set_read_buffer_mode(IOStream *this, IOStreamBufferMode mode);

void iostream_set_write_buffer_mode(IOStream *this, IOStreamBufferMode mode);

IOStream *iostream_open(const char *path, IOStreamFlag flags);

void iostream_close(IOStream *stream);

int iostream_read(IOStream *stream, void *buffer, uint size);

int iostream_write(IOStream *stream, const void *buffer, uint size);

int iostream_send(IOStream *stream);

int iostream_recv(IOStream *stream);

int iostream_flush(IOStream *stream);

int iostream_call(IOStream *stream, int request, void *arg);

int iostream_seek(IOStream *stream, int offset, IOStreamWhence whence);

int iostream_tell(IOStream *stream, IOStreamWhence whence);

int iostream_stat(IOStream *stream, IOStreamState *stat);

int iostream_puts(IOStream *stream, const char *string);

#define puts(__string) iostream_puts(out_stream, __string)

char *iostream_gets(IOStream *stream, char *string, int n);

int iostream_putchar(IOStream *stream, int c);

int iostream_getchar(IOStream *stream);

int iostream_ungetchar(IOStream *stream, char c);

int iostream_printf(IOStream *stream, const char *fmt, ...);

#define printf(__args...) iostream_printf(out_stream, __args)

int iostream_vprintf(IOStream *stream, const char *fmt, va_list va);

#define vprintf(__fmt, __va) iostream_vprintf(out_stream, __fmt, __va)

int iostream_scanf(IOStream *stream, const char *fmt, ...);

int iostream_vscanf(IOStream *stream, const char *fmt, va_list va);