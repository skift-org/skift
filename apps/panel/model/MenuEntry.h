#pragma once

#include <libgraphic/Icon.h>
#include <libutils/String.h>
#include <libutils/Vector.h>
#include <libutils/json/Json.h>

namespace panel
{

struct MenuEntry
{
    String id;
    String name;
    String comment;
    RefPtr<Icon> icon;
    RefPtr<Bitmap> image;
    String command;

    MenuEntry(String id, const json::Value &value);

    static Vector<MenuEntry> load();
};

} // namespace panel
