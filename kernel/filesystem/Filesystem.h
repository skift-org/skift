#pragma once

#include <libsystem/io/Path.h>
#include <libsystem/utils/List.h>

#include "kernel/node/Handle.h"
#include "kernel/node/Node.h"

void filesystem_initialize();

RefPtr<FsNode> filesystem_find_and_ref(Path *path);

RefPtr<FsNode> filesystem_find_parent_and_ref(Path *path);

ResultOr<FsHandle *> filesystem_open(Path *path, OpenFlag flags);

ResultOr<FsHandle *> filesystem_connect(Path *path);

Result filesystem_mkdir(Path *path);

Result filesystem_mkpipe(Path *path);

Result filesystem_mklink(Path *old_path, Path *new_path);

Result filesystem_mklink_for_tar(Path *old_path, Path *new_path);

Result filesystem_link_cstring(const char *path, RefPtr<FsNode> node);

Result filesystem_link(Path *path, RefPtr<FsNode> node);

Result filesystem_unlink(Path *path);

Result filesystem_rename(Path *old_path, Path *new_path);
