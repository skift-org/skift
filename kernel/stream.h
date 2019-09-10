#pragma once

#include <libsystem/iostream.h>

struct fsnode;

typedef struct
{
    int count;
    iostream_direntry_t *entries;
} directory_entries_t;

typedef struct
{
    void *p;
    int count;

    int current;
} stream_attached_info_t;

typedef struct stream
{
    struct fsnode *node;
    uint offset;
    iostream_flag_t flags;

    union {
        directory_entries_t direntries;
        stream_attached_info_t attached;
    };
} stream_t;

stream_t *stream(struct fsnode *node, iostream_flag_t flags);

void stream_delete(stream_t *s);
