#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* stream.h: generic io interface                                           */

#include <libsystem/io/Handle.h>
#include <libsystem/path.h>
#include <libsystem/runtime.h>

#include <abi/Filesystem.h>
#include <abi/IOCall.h>

typedef enum
{
    STREAM_BUFFERED_NONE,
    STREAM_BUFFERED_LINE,
    STREAM_BUFFERED_BLOCK,
} StreamBufferMode;

#define STREAM_BUFFER_SIZE 512

typedef struct Stream
{
    Handle handle;

    StreamBufferMode read_mode;
    void *write_buffer;
    int write_used;

    StreamBufferMode write_mode;
    void *read_buffer;
    int read_used;
    int read_head;

    bool has_unget;
    int unget_char;
} Stream;

extern Stream *in_stream;
extern Stream *out_stream;
extern Stream *err_stream;
extern Stream *log_stream;

Stream *stream_open(const char *path, OpenFlag flags);

Stream *stream_open_handle(int handle_id, OpenFlag flags);

Result stream_create_pipe(Stream **reader, Stream **writer);

Result stream_create_term(Stream **master, Stream **slave);

void stream_close(Stream *stream);

void stream_set_read_buffer_mode(Stream *stream, StreamBufferMode mode);

void stream_set_write_buffer_mode(Stream *stream, StreamBufferMode mode);

size_t stream_read(Stream *stream, void *buffer, size_t size);

size_t stream_write(Stream *stream, const void *buffer, size_t size);

void stream_flush(Stream *stream);

Result stream_call(Stream *stream, int request, void *arg);

int stream_seek(Stream *stream, int offset, Whence whence);

int stream_tell(Stream *stream, Whence whence);

void stream_stat(Stream *stream, FileState *stat);

int stream_putchar(Stream *stream, char c);

char stream_getchar(Stream *stream);

int stream_ungetchar(Stream *stream, char c);

int stream_printf(Stream *stream, const char *fmt, ...);

int stream_vprintf(Stream *stream, const char *fmt, va_list va);

#define printf(__args...) stream_printf(out_stream, __args)

#define vprintf(__fmt, __va) stream_vprintf(out_stream, __fmt, __va)
