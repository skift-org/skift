#include <string.h>

#include <libfilepicker/model/FilesystemModel.h>
#include <libio/File.h>
#include <libio/Format.h>
#include <libjson/Json.h>

namespace FilePicker
{

static auto get_icon_for_node(String current_directory, IO::Directory::Entry &entry)
{
    if (entry.stat.type == HJ_FILE_TYPE_DIRECTORY)
    {
        auto manifest_path = IO::format("{}/{}/manifest.json", current_directory, entry.name);

        IO::File manifest_file{manifest_path, HJ_OPEN_READ};

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
    else if (entry.stat.type == HJ_FILE_TYPE_PIPE ||
             entry.stat.type == HJ_FILE_TYPE_DEVICE ||
             entry.stat.type == HJ_FILE_TYPE_SOCKET)
    {
        return Graphic::Icon::get("pipe");
    }
    else if (entry.stat.type == HJ_FILE_TYPE_TERMINAL)
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

FilesystemModel::FilesystemModel(RefPtr<Navigation> navigation, Func<bool(IO::Directory::Entry &)> filter)
    : _navigation(navigation), _filter(filter)
{
    _observer = navigation->observe([this](auto &) {
        update();
    });

    update();
}

int FilesystemModel::rows()
{
    return _files.count();
}

int FilesystemModel::columns()
{
    return __COLUMN_COUNT;
}

String FilesystemModel::header(int column)
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

Widget::Variant FilesystemModel::data(int row, int column)
{
    auto &entry = _files[row];

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

        case HJ_FILE_TYPE_DEVICE:
            return "Device";

        default:
            return "Special file";
        }

    case COLUMN_SIZE:
        if (entry.type == HJ_FILE_TYPE_DIRECTORY)
        {
            return Widget::Variant(IO::format("{} Items", entry.size));
        }
        else
        {
            return Widget::Variant(IO::format("{} Bytes", entry.size));
        }

    default:
        ASSERT_NOT_REACHED();
    }
}

void FilesystemModel::update()
{
    _files.clear();

    IO::Directory directory{_navigation->current()};

    if (!directory.exist())
    {
        return;
    }

    for (auto entry : directory.entries())
    {
        if (_filter && !_filter(entry))
        {
            continue;
        }

        size_t size = entry.stat.size;

        if (entry.stat.type == HJ_FILE_TYPE_DIRECTORY)
        {
            auto path = IO::Path::join(_navigation->current(), entry.name);
            IO::Directory subdirectory{path};
            size = subdirectory.entries().count();
        }

        FileInfo node{
            .name = entry.name,
            .type = entry.stat.type,
            .icon = get_icon_for_node(_navigation->current().string(), entry),
            .size = size,
        };

        _files.push_back(node);
    }

    did_update();
}

const FileInfo &FilesystemModel::info(int index) const
{
    return _files[index];
}

} // namespace FilePicker