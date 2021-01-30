#include <abi/Filesystem.h>

#include <libgraphic/Icon.h>
#include <libutils/String.h>

namespace filepicker
{

struct ArchiveEntryInfo
{
    String name;
    FileType type;
    RefPtr<Icon> icon;
    size_t compressed_size;
    size_t uncompressed_size;
};

} // namespace filepicker