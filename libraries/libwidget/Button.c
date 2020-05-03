#include <libgraphic/Painter.h>
#include <libsystem/cstring.h>
#include <libwidget/Button.h>
#include <libwidget/Window.h>

void button_paint(Button *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (window_is_focused(WIDGET(widget)->window))
    {
        painter_fill_rectangle(painter, widget_bound(widget), widget_get_color(widget, THEME_MIDDLEGROUND));
    }
    painter_draw_rectangle(painter, widget_bound(widget), widget_get_color(widget, THEME_BORDER));

    int text_width = font_measure_string(widget_font(), widget->text);

    painter_draw_string(
        painter,
        widget_font(),
        widget->text,
        (Point){
            widget_bound(widget).X + widget_bound(widget).width / 2 - text_width / 2,
            widget_bound(widget).Y + widget_bound(widget).height / 2 + 4,
        },
        widget_get_color(widget, THEME_FOREGROUND));
}

void button_destroy(Button *widget)
{
    free(widget->text);
}

Widget *button_create(Widget *parent, const char *text)
{
    Button *widget = __create(Button);

    widget->text = strdup(text);

    WIDGET(widget)->paint = (WidgetPaintCallback)button_paint;
    WIDGET(widget)->destroy = (WidgetDestroyCallback)button_destroy;

    widget_initialize(WIDGET(widget), "Button", parent);

    return WIDGET(widget);
}
