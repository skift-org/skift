#include <libio/Directory.h>
#include <libio/File.h>
#include <libio/Format.h>
#include <libjson/Json.h>

#include "panel/model/MenuEntry.h"

MenuEntry::MenuEntry(String id, const Json::Value &value)
{
    value.with("name", [this](auto &v) {
        name = v.as_string();
    });

    value.with("comment", [this](auto &v) {
        comment = v.as_string();
    });

    value.with("icon", [this](auto &v) {
        icon = Graphic::Icon::get(v.as_string());
    });

    value.with("command", [this](auto &v) {
        command = v.as_string();
    });

    auto bitmap = Graphic::Bitmap::load_from(IO::format("/Applications/{}/{}.png", id, id));

    if (!bitmap.success())
    {
        bitmap = Graphic::Bitmap::load_from("/Files/missing.png");
    }

    image = bitmap.unwrap();
}

Vector<MenuEntry> MenuEntry::load()
{
    static Vector<MenuEntry> entries;

    if (entries.count() != 0)
    {
        return entries;
    }

    IO::Directory directory{"/Applications"};

    for (auto &entry : directory.entries())
    {
        if (entry.stat.type != HJ_FILE_TYPE_DIRECTORY)
        {
            continue;
        }

        auto path = IO::format("/Applications/{}/manifest.json", entry.name);

        IO::File manifest_file{path, HJ_OPEN_READ};

        if (manifest_file.exist())
        {
            entries.emplace_back(entry.name, Json::parse(manifest_file));
        }
    }

    entries.sort([](auto &a, auto &b) {
        return strcmp(a.name.cstring(), b.name.cstring());
    });

    return entries;
}
