#pragma once

/* stream.h: generic io interface                                           */

#include <abi/Filesystem.h>
#include <abi/IOCall.h>
#include <abi/Paths.h>

#include <libio/Seek.h>
#include <libsystem/io/Handle.h>

enum StreamBufferMode
{
    STREAM_BUFFERED_NONE,
    STREAM_BUFFERED_LINE,
    STREAM_BUFFERED_BLOCK,
};

#define STREAM_BUFFER_SIZE 512

struct Stream;

#ifndef __KERNEL__
Stream *__stream_get_in_stream();
Stream *__stream_get_out_stream();
Stream *__stream_get_err_stream();
Stream *__stream_get_log_stream();

#    define in_stream (__stream_get_in_stream())
#    define out_stream (__stream_get_out_stream())
#    define err_stream (__stream_get_err_stream())
#    define log_stream (__stream_get_log_stream())
#else
extern Stream *in_stream;
extern Stream *out_stream;
extern Stream *err_stream;
extern Stream *log_stream;
#endif

Stream *stream_open(const char *path, OpenFlag flags);

Stream *stream_open_handle(int handle_id, OpenFlag flags);

Result stream_create_term(Stream **server, Stream **client);

void stream_close(Stream *stream);

void stream_cleanup(Stream **stream);

void stream_set_read_buffer_mode(Stream *stream, StreamBufferMode mode);

void stream_set_write_buffer_mode(Stream *stream, StreamBufferMode mode);

size_t stream_read(Stream *stream, void *buffer, size_t size);

size_t stream_write(Stream *stream, const void *buffer, size_t size);

void stream_flush(Stream *stream);

Result stream_call(Stream *stream, IOCall request, void *arg);

int stream_seek(Stream *stream, IO::SeekFrom from);

int stream_tell(Stream *stream);

void stream_stat(Stream *stream, FileState *stat);

int stream_putchar(Stream *stream, char c);

char stream_getchar(Stream *stream);

int stream_ungetchar(Stream *stream, char c);

bool stream_is_end_file(Stream *stream);

int stream_format(Stream *stream, const char *fmt, ...);

int stream_vprintf(Stream *stream, const char *fmt, va_list va);