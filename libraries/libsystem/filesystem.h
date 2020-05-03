#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>

Result filesystem_link(const char *oldpath, const char *newpath);

Result filesystem_unlink(const char *path);

Result filesystem_mkdir(const char *path);

Result filesystem_mkpipe(const char *path);

Result filesystem_rename(const char *old_path, const char *new_path);
