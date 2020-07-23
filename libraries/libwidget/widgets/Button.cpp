#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>

#include <libwidget/Widgets.h>
#include <libwidget/Window.h>

void button_paint(Button *widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    if (widget_is_enable(WIDGET(widget)))
    {
        if (widget->style == BUTTON_OUTLINE)
        {
            painter_draw_rounded_rectangle(painter, widget_get_bound(widget), 4, 1, widget_get_color(widget, THEME_BORDER));
        }
        else if (widget->style == BUTTON_FILLED)
        {
            painter_fill_rounded_rectangle(painter, widget_get_bound(widget), 4, widget_get_color(widget, THEME_ACCENT));
        }

        if (widget->state == BUTTON_OVER)
        {
            painter_fill_rounded_rectangle(painter, widget_get_bound(widget), 4, ALPHA(widget_get_color(widget, THEME_FOREGROUND), 0.1));
        }

        if (widget->state == BUTTON_PRESS)
        {
            painter_fill_rounded_rectangle(painter, widget_get_bound(widget), 4, ALPHA(widget_get_color(widget, THEME_FOREGROUND), 0.2));
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

        Event action_event = {};

        action_event.type = EVENT_ACTION;

        widget_event(WIDGET(widget), &action_event);
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

    WIDGET(widget)->layout = HFLOW(0);
    WIDGET(widget)->insets = INSETS(0, 16);
    WIDGET(widget)->min_height = 32;
    WIDGET(widget)->layout_attributes |= LAYOUT_GREEDY;

    widget_initialize(WIDGET(widget), &button_class, parent);

    return WIDGET(widget);
}

Widget *button_create_with_icon(Widget *parent, ButtonStyle style, Icon *icon)
{
    Widget *button = button_create(parent, style);

    WIDGET(button)->layout = STACK();
    WIDGET(button)->insets = INSETS(4, 4);

    icon_panel_create(button, icon)->layout_attributes = LAYOUT_FILL;

    return button;
}

Widget *button_create_with_text(Widget *parent, ButtonStyle style, const char *text)
{
    Widget *button = button_create(parent, style);

    button->insets = INSETS(0, 0, 8, 8);
    button->min_width = 64;

    label_create(button, text)->layout_attributes = LAYOUT_FILL;

    return button;
}

Widget *button_create_with_icon_and_text(Widget *parent, ButtonStyle style, Icon *icon, const char *text)
{
    Widget *button = button_create(parent, style);
    button->insets = INSETS(0, 0, 6, 10);
    button->min_width = 64;

    Widget *button_icon = icon_panel_create(button, icon);
    button_icon->insets = INSETS(0, 0, 0, 4);

    label_create(button, text);

    return button;
}
