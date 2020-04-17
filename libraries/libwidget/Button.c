#include <libgraphic/Painter.h>
#include <libsystem/cstring.h>

#include <libwidget/Button.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>

void button_paint(Button *button, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (window_is_focused(WIDGET(button)->window))
    {
        painter_fill_rectangle(painter, widget_bound(button), THEME_ALT_BACKGROUND);
    }
    painter_draw_rectangle(painter, widget_bound(button), THEME_BORDER);

    int text_width = font_mesure_string(widget_font(), button->text);

    painter_draw_string(
        painter,
        widget_font(),
        button->text,
        (Point){
            widget_bound(button).X + widget_bound(button).width / 2 - text_width / 2,
            widget_bound(button).Y + widget_bound(button).height / 2 + 4,
        },
        THEME_FOREGROUND);
}

void button_destroy(Button *button)
{
    free(button->text);
}

Widget *button_create(Widget *parent, const char *text)
{
    Button *button = __create(Button);

    button->text = strdup(text);

    WIDGET(button)->paint = (WidgetPaintCallback)button_paint;
    WIDGET(button)->destroy = (WidgetDestroyCallback)button_destroy;

    widget_initialize(WIDGET(button), "Button", parent);

    return WIDGET(button);
}
