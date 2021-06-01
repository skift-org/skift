#pragma once

#include <abi/Filesystem.h>

#include <abi/Result.h>

HjResult filesystem_link(const char *oldpath, const char *newpath);

HjResult filesystem_unlink(const char *path);

HjResult filesystem_mkdir(const char *path);

HjResult filesystem_mkpipe(const char *path);

HjResult filesystem_rename(const char *old_path, const char *new_path);

bool filesystem_exist(const char *path, HjFileType type);
