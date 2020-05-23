#include <libgraphic/Painter.h>
#include <libwidget/Icon.h>
#include <libwidget/IconCache.h>
#include <libwidget/Window.h>

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
        return rectangle_expand(bitmap_bound(widget->bitmap), WIDGET(widget)->insets).size;
    }
    else
    {
        return widget_get_bound(widget).size;
    }
}

Widget *icon_create(Widget *parent, const char *name)
{
    Icon *icon = __create(Icon);

    icon->bitmap = icon_cache_get_icon(name);
    icon->bitmap->filtering = BITMAP_FILTERING_LINEAR;

    WIDGET(icon)->paint = (WidgetPaintCallback)icon_paint;
    WIDGET(icon)->size = (WidgetComputeSizeCallback)icon_size;

    widget_initialize(WIDGET(icon), "Icon", parent);

    return WIDGET(icon);
}