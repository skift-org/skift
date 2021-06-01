#include <string.h>

#include <libfilepicker/model/ArchiveListing.h>
#include <libjson/Json.h>

namespace FilePicker
{

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

Widget::Variant ArchiveListing::data(int row, int column)
{
    auto &entry = _entries[row];

    switch (column)
    {
    case COLUMN_NAME:
        return Widget::Variant(entry.name.cstring()).with_icon(entry.icon);

    case COLUMN_TYPE:
        switch (entry.type)
        {
        case HJ_FILE_TYPE_REGULAR:
            return "Regular file";

        case HJ_FILE_TYPE_DIRECTORY:
            return "Directory";

        default:
            return "Special file";
        }

    case COLUMN_COMPRESSED_SIZE:
        return Widget::Variant((int)entry.compressed_size);

    case COLUMN_UNCOMPRESSED_SIZE:
        return Widget::Variant((int)entry.uncompressed_size);

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
        entry_info.type = HJ_FILE_TYPE_REGULAR;
        entry_info.name = entry.name;
        entry_info.icon = Graphic::Icon::get("file");
    }

    did_update();
}

const ArchiveEntryInfo &ArchiveListing::info(int index) const
{
    return _entries[index];
}

} // namespace FilePicker