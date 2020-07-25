#include <libgraphic/Painter.h>
#include <libsystem/core/CString.h>
#include <libwidget/Window.h>
#include <libwidget/widgets/Placeholder.h>

void placeholder_paint(Placeholder *placeholder, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);
    painter_draw_rectangle(painter, widget_get_bound(placeholder), COLOR_RED);

    Icon *alert_icon = icon_get("alert");

    painter_blit_icon(
        painter,
        alert_icon,
        ICON_18PX,
        icon_bound(alert_icon, ICON_18PX).moved(widget_get_bound(placeholder).position() + Vec2i(8, 8)),
        COLOR_RED);

    painter_draw_string(
        painter,
        widget_font(),
        placeholder->text,
        widget_get_bound(placeholder).position() + Vec2i(32, 10),
        widget_get_color(placeholder, THEME_FOREGROUND));
}

void placeholder_destroy(Placeholder *placeholder)
{
    free(placeholder->text);
}

static const WidgetClass placeholder_class = {
    .name = "Placeholder",

    .destroy = (WidgetDestroyCallback)placeholder_destroy,
    .paint = (WidgetPaintCallback)placeholder_paint,
};

Widget *placeholder_create(Widget *parent, const char *text)
{
    Placeholder *placeholder = __create(Placeholder);

    char buffer[256];
    snprintf(buffer, 256, "Cannot create an instance of \"%s\".", text);

    placeholder->text = strdup(buffer);

    widget_initialize(WIDGET(placeholder), &placeholder_class, parent);

    return WIDGET(placeholder);
}
