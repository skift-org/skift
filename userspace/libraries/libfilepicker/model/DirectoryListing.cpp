#include <string.h>

#include <libfilepicker/model/DirectoryListing.h>
#include <libio/Directory.h>
#include <libio/File.h>
#include <libio/Format.h>
#include <libjson/Json.h>

namespace FilePicker
{

enum Column
{
    COLUMN_NAME,
    COLUMN_SIZE,

    __COLUMN_COUNT,
};

DirectoryListing::DirectoryListing(RefPtr<Navigation> navigation)
    : _navigation(navigation)
{
    _observer = navigation->observe([this](auto &) {
        update();
    });

    update();
}

int DirectoryListing::rows()
{
    return _directories.count();
}

int DirectoryListing::columns()
{
    return __COLUMN_COUNT;
}

String DirectoryListing::header(int column)
{
    switch (column)
    {
    case COLUMN_NAME:
        return "Name";

    case COLUMN_SIZE:
        return "Size";

    default:
        ASSERT_NOT_REACHED();
    }
}

Widget::Variant DirectoryListing::data(int row, int column)
{
    auto &entry = _directories[row];

    switch (column)
    {
    case COLUMN_NAME:
        return Widget::Variant(entry.name.cstring()).with_icon(entry.icon);

    case COLUMN_SIZE:
        return Widget::Variant(IO::format("{} Items", entry.size));

    default:
        ASSERT_NOT_REACHED();
    }
}

void DirectoryListing::update()
{
    _directories.clear();

    IO::Directory directory{_navigation->current()};

    if (!directory.exist())
    {
        return;
    }

    for (auto entry : directory.entries())
    {
        if (entry.stat.type != FileType::FILE_TYPE_DIRECTORY)
        {
            continue;
        }

        auto path = IO::Path::join(_navigation->current(), entry.name);
        IO::Directory subdirectory{path};

        DirectoryInfo node{
            .name = entry.name,
            .icon = Graphic::Icon::get("folder"),
            .size = subdirectory.entries().count(),
        };

        _directories.push_back(node);
    }

    did_update();
}

const DirectoryInfo &DirectoryListing::info(int index) const
{
    return _directories[index];
}

} // namespace FilePicker