#include <libsystem/cstring.h>
#include <libsystem/utils/HashMap.h>
#include <libwidget/IconCache.h>

static HashMap *_icons = NULL;

Bitmap *icon_cache_get_icon(const char *name)
{
    if (!_icons)
    {
        _icons = hashmap_create_string_to_value();
    }

    if (!hashmap_has(_icons, name))
    {
        char path[512];

        snprintf(path, 512, "/res/icons/%s.png", name);

        hashmap_put(_icons, name, bitmap_load_from(path));
    }

    return (Bitmap *)hashmap_get(_icons, name);
}
