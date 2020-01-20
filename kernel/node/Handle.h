#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "node/Node.h"

typedef struct FsHandle
{
    Lock lock;
    FsNode *node;
    size_t offset;
    OpenFlag flags;

    void *attached;
    size_t attached_size;
} FsHandle;

bool fshandle_has_flag(FsHandle *handle, OpenFlag flag);

FsHandle *fshandle_create(FsNode *node, OpenFlag flags);
void fshandle_destroy(FsHandle *handle);
FsHandle *fshandle_clone(FsHandle *handle);

bool fshandle_is_locked(FsHandle *handle);
void fshandle_acquire_lock(FsHandle *handle, int who_acquire);
void fshandle_release_lock(FsHandle *handle, int who_release);

int fshandle_read(FsHandle *handle, void *buffer, size_t size);
int fshandle_write(FsHandle *handle, const void *buffer, size_t size);

off_t fshandle_seek(FsHandle *handle, Whence whence, off_t where);
off_t fshandle_tell(FsHandle *handle, Whence whence);

int fshandle_call(FsHandle *handle, int request, void *args);
int fshandle_stat(FsHandle *handle, FileState *stat);
