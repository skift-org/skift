#pragma once

#include "kernel/node/Node.h"

struct FsHandle
{
    Lock lock;
    FsNode *node;
    size_t offset;
    OpenFlag flags;

    void *attached;
    size_t attached_size;
};

bool fshandle_has_flag(FsHandle *handle, OpenFlag flag);

FsHandle *fshandle_create(FsNode *node, OpenFlag flags);

void fshandle_destroy(FsHandle *handle);
FsHandle *fshandle_clone(FsHandle *handle);
SelectEvent fshandle_select(FsHandle *handle, SelectEvent events);

bool fshandle_is_locked(FsHandle *handle);
void fshandle_acquire_lock(FsHandle *handle, int who_acquire);
void fshandle_release_lock(FsHandle *handle, int who_release);

Result fshandle_read(FsHandle *handle, void *buffer, size_t size, size_t *read);
Result fshandle_write(FsHandle *handle, const void *buffer, size_t size, size_t *written);

Result fshandle_seek(FsHandle *handle, int offset, Whence whence);
Result fshandle_tell(FsHandle *handle, Whence whence, int *offset);

Result fshandle_call(FsHandle *handle, IOCall request, void *args);
Result fshandle_stat(FsHandle *handle, FileState *stat);

Result fshandle_connect(FsNode *node, FsHandle **connection_handle);
Result fshandle_accept(FsHandle *handle, FsHandle **connection_handle);
