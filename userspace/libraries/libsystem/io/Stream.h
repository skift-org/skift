#pragma once

/* stream.h: generic io interface                                           */

#include <abi/Filesystem.h>
#include <abi/IOCall.h>
#include <abi/Paths.h>

#include <libio/Handle.h>
#include <libio/Seek.h>

#define STREAM_BUFFER_SIZE 512

struct Stream
{
    Handle handle;

    bool has_unget;
    int unget_char;

    bool is_end_of_file;
};

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

void stream_close(Stream *stream);

void stream_cleanup(Stream **stream);

size_t stream_read(Stream *stream, void *buffer, size_t size);

size_t stream_write(Stream *stream, const void *buffer, size_t size);

Result stream_call(Stream *stream, IOCall request, void *arg);

int stream_seek(Stream *stream, IO::SeekFrom from);

void stream_stat(Stream *stream, FileState *stat);

int stream_format(Stream *stream, const char *fmt, ...);

int stream_vprintf(Stream *stream, const char *fmt, va_list va);
