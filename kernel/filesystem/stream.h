#pragma once

#include <libsystem/iostream.h>

struct fsnode;

typedef struct
{
    int count;
    IOStreamDirentry *entries;
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
    IOStreamFlag flags;

    union {
        directory_entries_t direntries;
        stream_attached_info_t attached;
    };
} stream_t;

stream_t *stream(struct fsnode *node, IOStreamFlag flags);

void stream_delete(stream_t *s);
