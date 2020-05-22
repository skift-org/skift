#include <libgraphic/Painter.h>
#include <libsystem/cstring.h>
#include <libwidget/Button.h>
#include <libwidget/Window.h>

void button_paint(Button *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (widget_is_enable(WIDGET(widget)))
    {
        if (widget->state == BUTTON_OVER)
        {
            painter_fill_rectangle(painter, widget_bound(widget), ALPHA(widget_get_color(widget, THEME_FOREGROUND), 0.025));
        }

        if (widget->state == BUTTON_PRESS)
        {
            painter_fill_rectangle(painter, widget_bound(widget), ALPHA(widget_get_color(widget, THEME_FOREGROUND), 0.05));
        }
    }

    int text_width = font_measure_string(widget_font(), widget->text);

    painter_draw_string(
        painter,
        widget_font(),
        widget->text,
        vec2i(
            widget_bound(widget).x + widget_bound(widget).width / 2 - text_width / 2,
            widget_bound(widget).y + widget_bound(widget).height / 2 + 4),
        widget_get_color(widget, THEME_FOREGROUND));
}

void button_destroy(Button *widget)
{
    free(widget->text);
}

void button_event(Button *widget, Event *event)
{
    if (event->type == EVENT_MOUSE_ENTER)
    {
        widget->state = BUTTON_OVER;
    }
    else if (event->type == EVENT_MOUSE_LEAVE)
    {
        widget->state = BUTTON_IDLE;
    }
    else if (event->type == EVENT_MOUSE_BUTTON_PRESS)
    {
        widget->state = BUTTON_PRESS;
    }
    else if (event->type == EVENT_MOUSE_BUTTON_RELEASE)
    {
        widget->state = BUTTON_OVER;
    }

    widget_update(WIDGET(widget));
}

Widget *button_create(Widget *parent, const char *text)
{
    Button *widget = __create(Button);

    widget->text = strdup(text);

    WIDGET(widget)->paint = (WidgetPaintCallback)button_paint;
    WIDGET(widget)->destroy = (WidgetDestroyCallback)button_destroy;
    WIDGET(widget)->event = (WidgetEventCallback)button_event;

    widget_initialize(WIDGET(widget), "Button", parent);

    return WIDGET(widget);
}
