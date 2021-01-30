#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/json/Json.h>

#include <libfilepicker/model/ArchiveListing.h>

namespace filepicker
{
/*
static auto get_icon_for_node(String current_directory, DirectoryEntry *entry)
{
    if (entry->stat.type == FILE_TYPE_DIRECTORY)
    {
        char manifest_path[PATH_LENGTH];

        snprintf(manifest_path, PATH_LENGTH, "%s/%s/manifest.json", current_directory.cstring(), entry->name);

        auto root = json::parse_file(manifest_path);

        if (root.is(json::OBJECT))
        {
            auto icon_name = root.get("icon");

            if (icon_name.is(json::STRING))
            {
                return Icon::get(icon_name.as_string());
            }
        }

        return Icon::get("folder");
    }
    else if (entry->stat.type == FILE_TYPE_PIPE ||
             entry->stat.type == FILE_TYPE_DEVICE ||
             entry->stat.type == FILE_TYPE_SOCKET)
    {
        return Icon::get("pipe");
    }
    else if (entry->stat.type == FILE_TYPE_TERMINAL)
    {
        return Icon::get("console-network");
    }
    else
    {
        return Icon::get("file");
    }
}
*/

enum Column
{
    COLUMN_NAME,
    COLUMN_TYPE,
    COLUMN_COMPRESSED_SIZE,
    COLUMN_UNCOMPRESSED_SIZE,

    __COLUMN_COUNT,
};

ArchiveListing::ArchiveListing(RefPtr<Navigation> navigation, RefPtr<Archive> archive)
    : _navigation(navigation), _archive(archive)
{
    _observer = navigation->observe([this](auto &) {
        update();
    });

    update();
}

int ArchiveListing::rows()
{
    return _entries.count();
}

int ArchiveListing::columns()
{
    return __COLUMN_COUNT;
}

String ArchiveListing::header(int column)
{
    switch (column)
    {
    case COLUMN_NAME:
        return "Name";

    case COLUMN_TYPE:
        return "Type";

    case COLUMN_COMPRESSED_SIZE:
        return "Compressed Size";

    case COLUMN_UNCOMPRESSED_SIZE:
        return "Uncompressed Size";

    default:
        ASSERT_NOT_REACHED();
    }
}

Variant ArchiveListing::data(int row, int column)
{
    auto &entry = _entries[row];

    switch (column)
    {
    case COLUMN_NAME:
        return Variant(entry.name.cstring()).with_icon(entry.icon);

    case COLUMN_TYPE:
        switch (entry.type)
        {
        case FILE_TYPE_REGULAR:
            return "Regular file";

        case FILE_TYPE_DIRECTORY:
            return "Directory";

        default:
            return "Special file";
        }

    case COLUMN_COMPRESSED_SIZE:
        return Variant((int)entry.compressed_size);

    case COLUMN_UNCOMPRESSED_SIZE:
        return Variant((int)entry.uncompressed_size);

    default:
        ASSERT_NOT_REACHED();
    }
}

void ArchiveListing::update()
{
    _entries.clear();

    auto current = _navigation->current();

    // Filter this by current
    for (const auto &entry : _archive->entries())
    {
        auto &entry_info = _entries.emplace_back();
        entry_info.compressed_size = entry.compressed_size;
        entry_info.uncompressed_size = entry.uncompressed_size;
        entry_info.type = FILE_TYPE_REGULAR;
        entry_info.name = entry.name;
        entry_info.icon = Icon::get("file");
    }

    did_update();
}

const ArchiveEntryInfo &ArchiveListing::info(int index) const
{
    return _entries[index];
}

} // namespace filepicker