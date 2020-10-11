#pragma once

#include <abi/Filesystem.h>

#include <libsystem/Result.h>

Result filesystem_link(const char *oldpath, const char *newpath);

Result filesystem_unlink(const char *path);

Result filesystem_mkdir(const char *path);

Result filesystem_mkpipe(const char *path);

Result filesystem_rename(const char *old_path, const char *new_path);

bool filesystem_exist(const char *path, FileType type);
