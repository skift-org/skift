#include <libjson/Json.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/process/Process.h>

#include "file-manager/FileSystemModel.h"

static auto get_icon_for_node(const char *current_directory, DirectoryEntry *entry)
{
    if (entry->stat.type == FILE_TYPE_DIRECTORY)
    {
        char manifest_path[PATH_LENGTH];

        snprintf(manifest_path, PATH_LENGTH, "%s/%s/manifest.json", current_directory, entry->name);

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

enum Column
{
    COLUMN_NAME,
    COLUMN_TYPE,
    COLUMN_SIZE,

    __COLUMN_COUNT,
};

FileSystemModel::FileSystemModel(String path)
{
    _current_path = path;
    process_set_directory(_current_path.cstring());
    update();
}

int FileSystemModel::rows()
{
    return _files.count();
}

int FileSystemModel::columns()
{
    return __COLUMN_COUNT;
}

String FileSystemModel::header(int column)
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

Variant FileSystemModel::data(int row, int column)
{
    auto &entry = _files[row];

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

        case FILE_TYPE_DEVICE:
            return "Device";

        default:
            return "Special file";
        }

    case COLUMN_SIZE:
        return Variant((int)entry.size);

    default:
        ASSERT_NOT_REACHED();
    }
}

void FileSystemModel::update()
{
    _files.clear();

    auto directory = directory_open(_current_path.cstring(), OPEN_READ);

    if (handle_has_error(directory))
    {
        directory_close(directory);
        return;
    }

    DirectoryEntry entry;
    while (directory_read(directory, &entry) > 0)
    {
        FileSystemNode node{
            .name = {entry.name, FILE_NAME_LENGTH},
            .type = entry.stat.type,
            .icon = get_icon_for_node(_current_path.cstring(), &entry),
            .size = entry.stat.size,
        };

        _files.push_back(node);
    }

    directory_close(directory);

    did_update();
}

void FileSystemModel::navigate(Path path)
{
    _current_path = path.string();
    process_set_directory(_current_path.cstring());
    update();
}

String FileSystemModel::file_name(int index)
{
    return _files[index].name;
}

FileType FileSystemModel::file_type(int index)
{
    return _files[index].type;
}
