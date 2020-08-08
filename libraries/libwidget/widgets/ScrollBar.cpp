#include <libgraphic/Painter.h>
#include <libwidget/Event.h>
#include <libwidget/widgets/ScrollBar.h>

Rectangle scrollbar_button_down(ScrollBar *widget)
{
    return widget_get_bound(widget).take_bottom(widget_get_bound(widget).width());
}

Rectangle scrollbar_button_up(ScrollBar *widget)
{
    return widget_get_bound(widget).take_top(widget_get_bound(widget).width());
}

static Rectangle scrollbar_track(ScrollBar *widget)
{
    return widget_get_bound(widget).shrinked(Insets(widget_get_bound(widget).width(), 0));
}

static Rectangle scrollbar_thumb(ScrollBar *widget)
{
    Rectangle track = scrollbar_track(widget);

    int thumb_height = MIN(track.height() * (widget->thumb / (double)widget->track), track.height());
    int thump_position = track.height() * (widget->value / (double)widget->track);

    return Rectangle(
        track.x() + 4,
        track.y() + thump_position,
        track.width() - 8,
        thumb_height);
}

static void scrollbar_paint(ScrollBar *widget, Painter &painter, Rectangle rectangle)
{
    __unused(rectangle);

    painter.clear_rectangle(widget_get_bound(widget), widget_get_color(widget, THEME_MIDDLEGROUND));

    Rectangle track = scrollbar_track(widget);

    if (track.height() * (widget->thumb / (double)widget->track) < track.height())
    {
        painter.fill_rounded_rectangle(scrollbar_thumb(widget), 4, widget_get_color(widget, THEME_BORDER));
    }

    if (widget->value == 0)
    {
        painter.blit_icon(*Icon::get("chevron-up"), ICON_18PX, scrollbar_button_up(widget), widget_get_color(widget, THEME_BORDER));
    }
    else
    {
        painter.blit_icon(*Icon::get("chevron-up"), ICON_18PX, scrollbar_button_up(widget), widget_get_color(widget, THEME_FOREGROUND));
    }

    if (widget->value >= widget->track - widget->thumb)
    {
        painter.blit_icon(*Icon::get("chevron-down"), ICON_18PX, scrollbar_button_down(widget), widget_get_color(widget, THEME_BORDER));
    }
    else
    {
        painter.blit_icon(*Icon::get("chevron-down"), ICON_18PX, scrollbar_button_down(widget), widget_get_color(widget, THEME_FOREGROUND));
    }
}

static void scrollbar_scroll_to(ScrollBar *widget, Vec2i mouse_position)
{
    int new_value = mouse_position.y() - scrollbar_track(widget).y();
    widget->value = (new_value / (double)scrollbar_track(widget).height()) * widget->track - widget->thumb / 2;

    widget->value = clamp(widget->value, 0, widget->track - widget->thumb);

    Event event_value_changed = {};
    event_value_changed.type = Event::VALUE_CHANGE;
    widget_event(widget, &event_value_changed);

    widget->should_repaint();
}

static void scrollbar_scroll_thumb(ScrollBar *widget, Vec2i mouse_position)
{
    mouse_position = mouse_position - widget->mouse_origine;

    int new_value = mouse_position.y() - scrollbar_track(widget).y();
    widget->value = (new_value / (double)scrollbar_track(widget).height()) * widget->track;

    widget->value = clamp(widget->value, 0, widget->track - widget->thumb);

    Event event_value_changed = {};
    event_value_changed.type = Event::VALUE_CHANGE;
    widget_event(widget, &event_value_changed);

    widget->should_repaint();
}

static void scrollbar_event(ScrollBar *widget, Event *event)
{
    if (is_mouse_event(event))
    {
        MouseEvent mouse_event = event->mouse;

        if (event->type == Event::MOUSE_MOVE && mouse_event.buttons & MOUSE_BUTTON_LEFT)
        {
            scrollbar_scroll_thumb(widget, mouse_event.position);
        }
        else if (event->type == Event::MOUSE_BUTTON_PRESS)
        {
            if (!scrollbar_thumb(widget).containe(mouse_event.position))
            {
                scrollbar_scroll_to(widget, mouse_event.position);
            }

            widget->mouse_origine = mouse_event.position - scrollbar_thumb(widget).position();
        }

        event->accepted = true;
    }
}

static const WidgetClass scrollbar_class = {
    .name = "ScrollBar",

    .paint = (WidgetPaintCallback)scrollbar_paint,
    .event = (WidgetEventCallback)scrollbar_event,
};

ScrollBar *scrollbar_create(Widget *parent)
{
    auto scrollbar = __create(ScrollBar);

    scrollbar->track = 1024;
    scrollbar->value = 512;
    scrollbar->thumb = 128;

    widget_initialize(scrollbar, &scrollbar_class, parent);

    return scrollbar;
}
