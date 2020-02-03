#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

int filesystem_link(const char* oldpath, const char* newpath);

int filesystem_unlink(const char* path);

int filesystem_mkdir(const char* path);

int filesystem_mkpipe(const char* path);

int filesystem_rename(const char* old_path, const char* new_path);
