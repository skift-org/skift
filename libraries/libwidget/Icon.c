#include <libgraphic/Painter.h>
#include <libwidget/Icon.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>

void icon_paint(Icon *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (widget->bitmap)
    {
        Rectangle destination = rectangle_center_within(bitmap_bound(widget->bitmap), widget_bound(widget));

        if (window_is_focused(WIDGET(widget)->window))
        {
            painter_blit_icon(painter, widget->bitmap, destination, THEME_ICON);
        }
        else
        {
            painter_blit_icon(painter, widget->bitmap, destination, THEME_ICON_INACTIVE);
        }
    }
}

Point icon_size(Icon *widget)
{
    if (widget->bitmap)
    {
        return rectangle_expand(bitmap_bound(widget->bitmap), WIDGET(widget)->insets).size;
    }
    else
    {
        return widget_bound(widget).size;
    }
}

void icon_destroy(Icon *widget)
{
    if (widget->bitmap)
    {
        bitmap_destroy(widget->bitmap);
    }
}

Widget *icon_create(Widget *parent, const char *path)
{
    Icon *icon = __create(Icon);

    icon->bitmap = bitmap_load_from(path);

    WIDGET(icon)->paint = (WidgetPaintCallback)icon_paint;
    WIDGET(icon)->size = (WidgetComputeSizeCallback)icon_size;
    WIDGET(icon)->destroy = (WidgetDestroyCallback)icon_destroy;

    widget_initialize(WIDGET(icon), "Icon", parent);

    return WIDGET(icon);
}