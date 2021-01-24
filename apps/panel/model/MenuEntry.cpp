#include <libsystem/io/Directory.h>
#include <libsystem/io/File.h>

#include "panel/model/MenuEntry.h"

namespace panel
{

MenuEntry::MenuEntry(const json::Value &value)
{
    value.with("name", [this](auto &v) {
        name = v.as_string();
    });

    value.with("comment", [this](auto &v) {
        comment = v.as_string();
    });

    value.with("icon", [this](auto &v) {
        icon = Icon::get(v.as_string());
    });

    value.with("command", [this](auto &v) {
        command = v.as_string();
    });
}

Vector<MenuEntry> MenuEntry::load()
{
    static Optional<Vector<MenuEntry>> entries;

    if (entries)
    {
        return *entries;
    }

    entries = Vector<MenuEntry>{};

    Directory *directory = directory_open("/Applications", OPEN_READ);

    if (handle_has_error(directory))
    {
        directory_close(directory);
        return *entries;
    }

    DirectoryEntry entry = {};
    while (directory_read(directory, &entry) > 0)
    {
        if (entry.stat.type == FILE_TYPE_DIRECTORY)
        {
            char path[PATH_LENGTH];
            snprintf(path, PATH_LENGTH, "/Applications/%s/manifest.json", entry.name);

            File manifest_file{path};
            if (manifest_file.exist())
            {
                entries->emplace_back(json::parse_file(path));
            }
        }
    }

    entries->sort([](auto &a, auto &b) {
        return strcmp(a.name.cstring(), b.name.cstring());
    });

    directory_close(directory);

    return *entries;
}

} // namespace panel
