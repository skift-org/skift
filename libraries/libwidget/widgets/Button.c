#include <libgraphic/Painter.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include <libwidget/Widgets.h>
#include <libwidget/Window.h>

void button_paint(Button *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (widget_is_enable(WIDGET(widget)))
    {
        if (widget->state == BUTTON_OVER)
        {
            painter_fill_rounded_rectangle(painter, widget_get_bound(widget), 6, ALPHA(widget_get_color(widget, THEME_FOREGROUND), 0.1));
        }

        if (widget->state == BUTTON_PRESS)
        {
            painter_fill_rounded_rectangle(painter, widget_get_bound(widget), 6, ALPHA(widget_get_color(widget, THEME_FOREGROUND), 0.2));
        }

        if (WIDGET(widget)->window->mouse_focused_widget == WIDGET(widget))
        {
            painter_draw_rounded_rectangle(painter, widget_get_bound(widget), 6, 1, widget_get_color(widget, THEME_ACCENT));
        }
    }
}

void button_event(Button *widget, Event *event)
{
    if (event->type == EVENT_MOUSE_ENTER)
    {
        widget->state = BUTTON_OVER;
        widget_update(WIDGET(widget));
        event->accepted = true;
    }
    else if (event->type == EVENT_MOUSE_LEAVE)
    {
        widget->state = BUTTON_IDLE;
        widget_update(WIDGET(widget));
        event->accepted = true;
    }
    else if (event->type == EVENT_MOUSE_BUTTON_PRESS)
    {
        widget->state = BUTTON_PRESS;
        widget_update(WIDGET(widget));
        event->accepted = true;
    }
    else if (event->type == EVENT_MOUSE_BUTTON_RELEASE)
    {
        widget->state = BUTTON_OVER;
        widget_update(WIDGET(widget));
        event->accepted = true;

        widget_event(WIDGET(widget), &(Event){.type = EVENT_ACTION});
    }
}

static const WidgetClass button_class = {
    .name = "Button",

    .paint = (WidgetPaintCallback)button_paint,
    .event = (WidgetEventCallback)button_event,
};

Widget *button_create(Widget *parent, ButtonStyle style)
{
    Button *widget = __create(Button);

    widget->style = style;
    widget->state = BUTTON_IDLE;

    WIDGET(widget)->layout = HFLOW(4);
    WIDGET(widget)->insets = INSETS(0, 4);
    WIDGET(widget)->layout_attributes |= LAYOUT_GREEDY;

    widget_initialize(WIDGET(widget), &button_class, parent);

    return WIDGET(widget);
}

Widget *button_create_with_text(Widget *parent, ButtonStyle style, const char *text)
{
    Widget *button = button_create(parent, style);

    label_create(button, text)->layout_attributes = LAYOUT_FILL;

    return button;
}

Widget *button_create_with_icon(Widget *parent, ButtonStyle style, const char *icon)
{
    Widget *button = button_create(parent, style);

    WIDGET(button)->layout = STACK();
    WIDGET(button)->insets = INSETS(4, 4);

    icon_create(button, icon)->layout_attributes = LAYOUT_FILL;

    return button;
}

Widget *button_create_with_icon_and_text(Widget *parent, ButtonStyle style, const char *icon, const char *text)
{
    Widget *button = button_create(parent, style);

    icon_create(button, icon);
    label_create(button, text);

    return button;
}
