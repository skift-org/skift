#include <libgraphic/Icon.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/utils/HashMap.h>

static HashMap *_icons = nullptr;

#define ICON_SIZE_NAME_ENTRY(__size) #__size,
const char *_icon_size_names[] = {ICON_SIZE_LIST(ICON_SIZE_NAME_ENTRY)};

#define ICON_SIZES_ENTRY(__size) __size,
const int _icon_sizes[] = {ICON_SIZE_LIST(ICON_SIZES_ENTRY)};

static RefPtr<Icon> icon_load(String name)
{
    auto icon = make<Icon>(name);

    for (size_t i = 0; i < __ICON_SIZE_COUNT; i++)
    {
        char path[PATH_LENGTH] = {};
        snprintf(path, PATH_LENGTH, "/System/Icons/%s@%spx.png", name.cstring(), _icon_size_names[i]);

        auto bitmap_or_result = Bitmap::load_from(path);
        if (bitmap_or_result.success())
        {
            icon->set_bitmap(static_cast<IconSize>(i), bitmap_or_result.take_value());
        }
    }

    return icon;
}

RefPtr<Icon> Icon::get(String name)
{
    if (!_icons)
    {
        _icons = hashmap_create_string_to_value();
    }

    if (!hashmap_has(_icons, name.cstring()))
    {
        return icon_load(name);
    }
    else
    {
        return *(Icon *)hashmap_get(_icons, name.cstring());
    }
}

Icon::Icon(String name)
    : _name(name)
{
    hashmap_put(_icons, name.cstring(), this);
}

Icon::~Icon()
{
    hashmap_remove_value(_icons, this);
}

Rectangle Icon::bound(IconSize size)
{
    return bitmap(size)->bound();
}

RefPtr<Bitmap> Icon::bitmap(IconSize size)
{
    if (_bitmaps[size])
    {
        return _bitmaps[size];
    }

    for (size_t i = 0; i < __ICON_SIZE_COUNT; i++)
    {
        if (_bitmaps[i])
        {
            return _bitmaps[i];
        }
    }

    ASSERT_NOT_REACHED();
}

void Icon::set_bitmap(IconSize size, RefPtr<Bitmap> bitmap)
{
    _bitmaps[size] = bitmap;
}
