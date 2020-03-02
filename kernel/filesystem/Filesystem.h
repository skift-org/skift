#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/utils/List.h>
#include <libsystem/path.h>
#include <libsystem/runtime.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Node.h"
#include "kernel/system.h"

void filesystem_initialize(void);

FsNode *filesystem_find_and_ref(Path *path);

FsNode *filesystem_find_parent_and_ref(Path *path);

Result filesystem_open(Path *path, OpenFlag flags, FsHandle **handle);

Result filesystem_connect(Path *path, FsHandle **connection_handle);

Result filesystem_mkdir(Path *path);

Result filesystem_mkfile(Path *path);

Result filesystem_mkpipe(Path *path);

Result filesystem_mklink(Path *old_path, Path *new_path);

Result filesystem_link(Path *path, FsNode *node);

Result filesystem_link_and_take_ref(Path *path, FsNode *node);

Result filesystem_unlink(Path *path);

Result filesystem_rename(Path *old_path, Path *new_path);
