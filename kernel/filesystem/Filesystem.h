#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/list.h>
#include <libsystem/path.h>
#include <libsystem/runtime.h>

#include "node/Handle.h"
#include "node/Node.h"
#include "system.h"

void filesystem_initialize(void);

FsNode *filesystem_find_and_ref(Path *path);

FsNode *filesystem_find_parent_and_ref(Path *path);

error_t filesystem_open(Path *path, OpenFlag flags, FsHandle **handle);

error_t filesystem_connect(Path *path, FsHandle **connection_handle);

int filesystem_mkdir(Path *path);

int filesystem_mkfile(Path *path);

int filesystem_mkpipe(Path *path);

int filesystem_mklink(Path *old_path, Path *new_path);

int filesystem_link(Path *path, FsNode *node);

int filesystem_link_and_take_ref(Path *path, FsNode *node);

int filesystem_unlink(Path *path);

int filesystem_rename(Path *old_path, Path *new_path);
