#pragma once

#include <abi/Filesystem.h>

#include <libgraphic/Icon.h>
#include <libutils/String.h>

namespace filepicker
{

struct FileInfo
{
    String name;
    FileType type;
    RefPtr<Icon> icon;
    size_t size;
};

} // namespace filepicker