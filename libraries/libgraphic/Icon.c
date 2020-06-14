#include <libgraphic/Icon.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/utils/HashMap.h>

static HashMap *_icons;

#define ICON_SIZE_NAME_ENTRY(__size) #__size,
const char *_icon_size_names[] = {ICON_SIZE_LIST(ICON_SIZE_NAME_ENTRY)};

#define ICON_SIZES_ENTRY(__size) __size,
const int _icon_sizes[] = {ICON_SIZE_LIST(ICON_SIZES_ENTRY)};

static Icon *icon_load(const char *name)
{
    Icon *icon = __create(Icon);

    for (size_t i = 0; i < __ICON_SIZE_COUNT; i++)
    {
        char path[512] = {};
        snprintf(path, 512, "/res/icons/%s@%spx.png", name, _icon_size_names[i]);
        logger_trace("Loading icon from: %s", path);

        Bitmap *bitmap = NULL;
        if (bitmap_load_from_can_fail(path, &bitmap) != SUCCESS)
        {
            logger_warn("No icon of %spx in size", _icon_size_names[i]);
        }

        if (bitmap != NULL)
        {
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
