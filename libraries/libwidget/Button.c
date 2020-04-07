#include <libgraphic/Painter.h>
#include <libsystem/cstring.h>

#include <libwidget/Button.h>
#include <libwidget/Theme.h>

void button_paint(Button *button, Painter *painter)
{
    painter_fill_rectangle(painter, WIDGET(button)->bound, THEME_ALT_BACKGROUND);
    painter_draw_rectangle(painter, WIDGET(button)->bound, THEME_BORDER);
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
