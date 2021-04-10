#include <libwidget/Slider.h>

namespace Widget
{

Math::Recti Slider::track_bound()
{
    return Math::Recti{
        (bound().width() - THUMP_SIZE),
        bound().height(),
    }
        .with_height(TRACK_HEIGHT)
        .centered_within(bound());
}

Math::Recti Slider::value_bound()
{
    return track_bound()
        .take_left(track_bound().width() * _value);
}

Math::Recti Slider::thumb_bound()
{
    int posx = value_bound().right() - THUMP_SIZE / 2;

    return {
        posx,
        bound().height() / 2 - THUMP_SIZE / 2,
        THUMP_SIZE,
        THUMP_SIZE,
    };
}

void Slider::slide_to(Math::Vec2i position)
{
    Math::Vec2i pos = position - bound().position();
    _value = pos.x() / (double)bound().width();
    _value = clamp(_value, 0, 1);

    Event event_value_changed = {};
    event_value_changed.type = Event::VALUE_CHANGE;
    dispatch_event(&event_value_changed);

    should_repaint();
    should_relayout();
}

Slider::Slider(Component *parent) : Component(parent)
{
    min_width(160);
}

void Slider::event(Event *event)
{
    if (is_mouse_event(event))
    {
        MouseEvent mouse_event = event->mouse;

        if (event->type == Event::MOUSE_MOVE && mouse_event.buttons & MOUSE_BUTTON_LEFT)
        {
            slide_to(mouse_event.position);
            event->accepted = true;
        }
        else if (event->type == Event::MOUSE_BUTTON_PRESS)
        {
            slide_to(mouse_event.position);
            event->accepted = true;
        }
    }
}

void Slider::paint(Graphic::Painter &painter, const Math::Recti &)
{
    if (window()->focused())
    {
        painter.fill_rectangle(track_bound(), color(THEME_BORDER));
        painter.fill_rectangle(value_bound(), color(THEME_ACCENT));
        painter.fill_rectangle_rounded(thumb_bound(), bound().height() / 2, color(THEME_ACCENT));
    }
    else
    {
        painter.fill_rectangle(value_bound(), color(THEME_BACKGROUND));
        painter.fill_rectangle_rounded(thumb_bound(), bound().height() / 2, color(THEME_BACKGROUND));
    }
}

} // namespace Widget
