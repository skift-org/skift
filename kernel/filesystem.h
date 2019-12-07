#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>
#include <libsystem/path.h>
#include <libsystem/list.h>

#include "system.h"
#include "fsnode.h"
#include "stream.h"

#define ROOT NULL

void filesystem_setup(void);
void filesystem_panic_dump(void);

/* --- fsnode --------------------------------------------------------------- */

// Resolve and acquire a fsnode.
fsnode_t *filesystem_acquire(fsnode_t *at, Path *p, bool create);

// Release the fsnode.
void filesystem_release(fsnode_t *node);

/* --- File IO -------------------------------------------------------------- */

stream_t *filesystem_open(fsnode_t *at, Path *p, IOStreamFlag flags);

void filesystem_close(stream_t *s);

stream_t *filesystem_dup(stream_t *s);

int filesystem_read(stream_t *s, void *buffer, uint size);

int filesystem_write(stream_t *s, const void *buffer, uint size);

bool filesystem_can_read(stream_t * s);

bool filesystem_can_write(stream_t *s);

int filesystem_call(stream_t *s, int request, void *args);

int filesystem_seek(stream_t *s, int offset, IOStreamWhence origine);

int filesystem_tell(stream_t *s, IOStreamWhence whence);

int filesystem_stat(stream_t *s, IOStreamState *stat);

/* --- File system operation ------------------------------------------------ */

int filesystem_mkfile(fsnode_t *at, Path *p);

int filesystem_mkfifo(fsnode_t *at, Path *p); /* TODO */

int filesystem_mkdev(fsnode_t *at, Path *p, device_t dev);

int filesystem_mkdir(fsnode_t *at, Path *p);

int filesystem_link(fsnode_t *atfile, Path *file, fsnode_t *atlink, Path *link);

int filesystem_unlink(fsnode_t *at, Path *p);

int filesystem_rename(fsnode_t *atoldpath, Path *oldpath, fsnode_t *atnewpath, Path *newpath);

bool filesystem_exist(fsnode_t *at, Path *p);

// *filesystem_mkdev* with error checking.
#define FILESYSTEM_MKDEV(__name, __object)                       \
    {                                                            \
        Path *__dev_path = path(__name);                       \
        logger_info("Creating device " __name " at " __name);    \
        if (filesystem_mkdev(NULL, __dev_path, (__object)))      \
        {                                                        \
            PANIC("Failled to create the '" __name "' device."); \
        }                                                        \
        path_delete(__dev_path);                                 \
    }
