#include <libgraphic/Painter.h>
#include <libwidget/widgets/IconPanel.h>

void icon_panel_paint(IconPanel *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (!widget->icon)
    {
        return;
    }

    Rectangle destination = rectangle_center_within(
        icon_bound(widget->icon, ICON_18PX),
        widget_get_content_bound(widget));

    painter_blit_icon(
        painter,
        widget->icon,
        ICON_18PX,
        destination,
        widget_get_color(widget, THEME_FOREGROUND));
}

Vec2i icon_panel_size(IconPanel *widget)
{
    if (widget->icon)
    {
        return icon_bound(widget->icon, ICON_18PX).size;
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

Widget *icon_panel_create(Widget *parent, Icon *icon)
{
    IconPanel *widget = __create(IconPanel);

    widget->icon = icon;
    widget_initialize(WIDGET(widget), &icon_panel_class, parent);

    return WIDGET(widget);
}
