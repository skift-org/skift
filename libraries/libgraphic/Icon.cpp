#include <stdio.h>
#include <string.h>

#include <libgraphic/Icon.h>
#include <libio/Format.h>
#include <libutils/HashMap.h>
#include <libutils/Path.h>

namespace Graphic
{

static HashMap<String, RefPtr<Icon>>
    _icons{};

#define ICON_SIZE_NAME_ENTRY(__size) #__size,
const char *_icon_size_names[] = {ICON_SIZE_LIST(ICON_SIZE_NAME_ENTRY)};

#define ICON_SIZES_ENTRY(__size) __size,
const int _icon_sizes[] = {ICON_SIZE_LIST(ICON_SIZES_ENTRY)};

static RefPtr<Icon> icon_load(String name)
{
    auto icon = make<Icon>(name);

    for (size_t i = 0; i < __ICON_SIZE_COUNT; i++)
    {
        auto path = IO::format("/Files/Icons/{}@{}px.png", name, _icon_size_names[i]);
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
    if (!_icons.has_key(name))
    {
        _icons[name] = icon_load(name);
    }

    return _icons[name];
}

Icon::Icon(String name)
    : _name(name)
{
}

Icon::~Icon()
{
}

Recti Icon::bound(IconSize size)
{
    return bitmap(size)->bound();
}

RefPtr<Bitmap> Icon::bitmap(IconSize size)
{
    if (!_bitmaps[size])
    {
        for (size_t i = 0; i < __ICON_SIZE_COUNT; i++)
        {
            if (_bitmaps[i])
            {
                return _bitmaps[i];
            }
        }

        _bitmaps[size] = Bitmap::load_from_or_placeholder("none");
    }

    return _bitmaps[size];

    ASSERT_NOT_REACHED();
}

void Icon::set_bitmap(IconSize size, RefPtr<Bitmap> bitmap)
{
    _bitmaps[size] = bitmap;
}

} // namespace Graphic
