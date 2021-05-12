#pragma once

#include <libgraphic/Icon.h>
#include <libjson/Json.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

struct MenuEntry
{
    String id;
    String name;
    String comment;
    RefPtr<Graphic::Icon> icon;
    RefPtr<Graphic::Bitmap> image;
    String command;

    MenuEntry(String id, const Json::Value &value);

    static Vector<MenuEntry> load();
};
