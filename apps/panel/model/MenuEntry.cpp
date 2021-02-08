#include <libsystem/io/File.h>
#include <libsystem/io_new/Directory.h>

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

    System::Directory directory{"/Applications"};

    if (!directory.exist())
    {
        return *entries;
    }

    for (auto &entry : directory.entries())
    {
        if (entry.stat.type != FILE_TYPE_DIRECTORY)
        {
            continue;
        }

        auto path = String::format("/Applications/{}/manifest.json", entry.name);

        File manifest_file{path};
        if (manifest_file.exist())
        {
            entries->emplace_back(json::parse_file(path));
        }
    }

    entries->sort([](auto &a, auto &b) {
        return strcmp(a.name.cstring(), b.name.cstring());
    });

    return *entries;
}

} // namespace panel
