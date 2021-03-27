#pragma once

#include <abi/Filesystem.h>

#include <libgraphic/Icon.h>
#include <libutils/String.h>

namespace FilePicker
{

struct ArchiveEntryInfo
{
    String name;
    FileType type;
    RefPtr<Graphic::Icon> icon;
    size_t compressed_size;
    size_t uncompressed_size;
};

} // namespace FilePicker