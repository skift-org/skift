#include <string.h>

#include <libfilepicker/model/DirectoryListing.h>
#include <libio/Directory.h>
#include <libio/File.h>
#include <libio/Format.h>
#include <libutils/json/Json.h>

namespace FilePicker
{

static auto get_icon_for_node(String current_directory, IO::Directory::Entry &entry)
{
    if (entry.stat.type == FILE_TYPE_DIRECTORY)
    {
        auto manifest_path = IO::format("{}/{}/manifest.json", current_directory, entry.name);

        IO::File manifest_file{manifest_path, OPEN_READ};

        if (manifest_file.exist())
        {
            auto root = Json::parse(manifest_file);

            if (root.is(Json::OBJECT))
            {
                auto icon_name = root.get("icon");

                if (icon_name.is(Json::STRING))
                {
                    return Graphic::Icon::get(icon_name.as_string());
                }
            }
        }

        return Graphic::Icon::get("folder");
    }
    else if (entry.stat.type == FILE_TYPE_PIPE ||
             entry.stat.type == FILE_TYPE_DEVICE ||
             entry.stat.type == FILE_TYPE_SOCKET)
    {
        return Graphic::Icon::get("pipe");
    }
    else if (entry.stat.type == FILE_TYPE_TERMINAL)
    {
        return Graphic::Icon::get("console-network");
    }
    else
    {
        return Graphic::Icon::get("file");
    }
}

enum Column
{
    COLUMN_NAME,
    COLUMN_TYPE,
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
    return _files.count();
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

    case COLUMN_TYPE:
        return "Type";

    case COLUMN_SIZE:
        return "Size";

    default:
        ASSERT_NOT_REACHED();
    }
}

Widget::Variant DirectoryListing::data(int row, int column)
{
    auto &entry = _files[row];

    switch (column)
    {
    case COLUMN_NAME:
        return Widget::Variant(entry.name.cstring()).with_icon(entry.icon);

    case COLUMN_TYPE:
        switch (entry.type)
        {
        case FILE_TYPE_REGULAR:
            return "Regular file";

        case FILE_TYPE_DIRECTORY:
            return "Directory";

        case FILE_TYPE_DEVICE:
            return "Device";

        default:
            return "Special file";
        }

    case COLUMN_SIZE:
        return Widget::Variant((int)entry.size);

    default:
        ASSERT_NOT_REACHED();
    }
}

void DirectoryListing::update()
{
    _files.clear();

    IO::Directory directory{_navigation->current()};

    if (!directory.exist())
    {
        return;
    }

    for (auto entry : directory.entries())
    {
        FileInfo node{
            .name = entry.name,
            .type = entry.stat.type,
            .icon = get_icon_for_node(_navigation->current().string(), entry),
            .size = entry.stat.size,
        };

        _files.push_back(node);
    }

    did_update();
}

const FileInfo &DirectoryListing::info(int index) const
{
    return _files[index];
}

} // namespace FilePicker