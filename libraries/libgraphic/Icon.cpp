#include <libgraphic/Icon.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/utils/HashMap.h>

static HashMap *_icons;

#define ICON_SIZE_NAME_ENTRY(__size) #__size,
const char *_icon_size_names[] = {ICON_SIZE_LIST(ICON_SIZE_NAME_ENTRY)};

#define ICON_SIZES_ENTRY(__size) __size,
const int _icon_sizes[] = {ICON_SIZE_LIST(ICON_SIZES_ENTRY)};

static Icon *icon_load(const char *name)
{
    Icon *icon = __create(Icon);
    icon->name = strdup(name);

    for (size_t i = 0; i < __ICON_SIZE_COUNT; i++)
    {
        char path[PATH_LENGTH] = {};
        snprintf(path, PATH_LENGTH, "/System/Icons/%s@%spx.png", name, _icon_size_names[i]);

        Bitmap *bitmap = nullptr;
        if (bitmap_load_from_can_fail(path, &bitmap) == SUCCESS)
        {
            bitmap->filtering = BITMAP_FILTERING_LINEAR;
            icon->sizes[i] = bitmap;
        }
    }

    return icon;
}

Icon *icon_get(const char *name)
{
    if (!_icons)
    {
        _icons = hashmap_create_string_to_value();
    }

    if (!hashmap_has(_icons, name))
    {
        hashmap_put(_icons, name, icon_load(name));
    }

    return (Icon *)hashmap_get(_icons, name);
}

Rectangle icon_bound(Icon *icon, IconSize size)
{
    return bitmap_bound(icon_get_bitmap(icon, size));
}

Bitmap *icon_get_bitmap(Icon *icon, IconSize size)
{
    Bitmap *bitmap = icon->sizes[size];

    if (bitmap == nullptr)
    {
        for (size_t i = 0; i < __ICON_SIZE_COUNT; i++)
        {
            if (icon->sizes[i])
            {
                bitmap = icon->sizes[i];
            }
        }
    }

    return bitmap;
}
