#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "node/Node.h"

typedef struct Handle
{
    Lock lock;
    FsNode *node;
    size_t offset;
    IOStreamFlag flags;

    void *attached;
    size_t attached_size;
} Handle;

bool handle_has_flag(Handle *handle, IOStreamFlag flag);

Handle *handle_create(FsNode *node, IOStreamFlag flags);
void handle_destroy(Handle *handle);
Handle *handle_clone(Handle *handle);

bool handle_is_locked(Handle *handle);
void handle_acquire_lock(Handle *handle, int who_acquire);
void handle_release_lock(Handle *handle, int who_release);

int handle_read(Handle *handle, void *buffer, size_t size);
int handle_write(Handle *handle, const void *buffer, size_t size);

off_t handle_seek(Handle *handle, IOStreamWhence whence, off_t where);
off_t handle_tell(Handle *handle, IOStreamWhence whence);

int handle_call(Handle *handle, int request, void *args);
int handle_stat(Handle *handle, IOStreamState *stat);
