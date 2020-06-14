#include <libgraphic/Painter.h>
#include <libwidget/utils/IconCache.h>
#include <libwidget/widgets/IconPanel.h>

void icon_panel_paint(IconPanel *widget, Painter *painter, Rectangle rectangle)
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

Vec2i icon_panel_size(IconPanel *widget)
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

static const WidgetClass icon_panel_class = {
    .name = "IconPanel",

    .paint = (WidgetPaintCallback)icon_panel_paint,
    .size = (WidgetComputeSizeCallback)icon_panel_size,
};

Widget *icon_panel_create(Widget *parent, const char *name)
{
    IconPanel *icon = __create(IconPanel);

    icon->bitmap = icon_cache_get_icon(name);
    icon->bitmap->filtering = BITMAP_FILTERING_LINEAR;

    widget_initialize(WIDGET(icon), &icon_panel_class, parent);

    return WIDGET(icon);
}
