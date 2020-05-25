#include <libgraphic/Painter.h>
#include <libwidget/utils/IconCache.h>
#include <libwidget/widgets/Icon.h>

void icon_paint(Icon *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (widget->bitmap)
    {
        Rectangle destination = rectangle_center_within(bitmap_bound(widget->bitmap), widget_get_content_bound(widget));

        if (widget_is_enable(WIDGET(widget)))
        {
            painter_blit_icon(painter, widget->bitmap, destination, widget_get_color(widget, THEME_FOREGROUND));
        }
        else
        {
            painter_blit_icon(painter, widget->bitmap, destination, widget_get_color(widget, THEME_FOREGROUND_DISABLED));
        }
    }
}

Vec2i icon_size(Icon *widget)
{
    if (widget->bitmap)
    {
        return bitmap_bound(widget->bitmap).size;
    }
    else
    {
        return widget_get_bound(widget).size;
    }
}

static const WidgetClass icon_class = {
    .name = "Icon",

    .paint = (WidgetPaintCallback)icon_paint,
    .size = (WidgetComputeSizeCallback)icon_size,
};

Widget *icon_create(Widget *parent, const char *name)
{
    Icon *icon = __create(Icon);

    icon->bitmap = icon_cache_get_icon(name);
    icon->bitmap->filtering = BITMAP_FILTERING_LINEAR;

    widget_initialize(WIDGET(icon), &icon_class, parent);

    return WIDGET(icon);
}
