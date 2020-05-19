#include <libgraphic/Painter.h>
#include <libsystem/cstring.h>
#include <libwidget/IconCache.h>
#include <libwidget/Placeholder.h>
#include <libwidget/Window.h>

void placeholder_paint(Placeholder *placeholder, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);
    painter_draw_rectangle(painter, widget_bound(placeholder), COLOR_RED);

    painter_blit_icon(painter, icon_cache_get_icon("alert"), rectangle_offset(RECTANGLE_SIZE(18, 18), vec2i_add(widget_bound(placeholder).position, vec2i(8, 8))), COLOR_RED);

    painter_draw_string(
        painter,
        widget_font(),
        placeholder->text,
        vec2i(
            widget_bound(placeholder).x + 32,
            widget_bound(placeholder).y + 20),
        widget_get_color(placeholder, THEME_FOREGROUND));
}

void placeholder_destroy(Placeholder *placeholder)
{
    free(placeholder->text);
}

Widget *placeholder_create(Widget *parent, const char *text)
{
    Placeholder *placeholder = __create(Placeholder);

    char buffer[256];
    snprintf(buffer, 256, "Cannot create an instance of \"%s\".", text);

    placeholder->text = strdup(buffer);

    WIDGET(placeholder)->paint = (WidgetPaintCallback)placeholder_paint;
    WIDGET(placeholder)->destroy = (WidgetDestroyCallback)placeholder_destroy;

    widget_initialize(WIDGET(placeholder), "Placeholder", parent);

    return WIDGET(placeholder);
}
