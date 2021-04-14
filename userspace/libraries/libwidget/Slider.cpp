#include <libwidget/Slider.h>

namespace Widget
{

const auto THUMB_SIZE = 16;

int Slider::icon_size()
{
    return track_bound().height() - 8;
}

Math::Recti Slider::track_bound()
{
    return bound();
}

Math::Recti Slider::value_bound()
{
    return track_bound().take_left((track_bound().width() - THUMB_SIZE - (icon_size())) * _value + THUMB_SIZE + icon_size());
}

void Slider::slide_to(Math::Vec2i position)
{
    Math::Vec2i pos = position - bound().position() - THUMB_SIZE - icon_size() + THUMB_SIZE / 2;
    _value = pos.x() / (double)(bound().width() - THUMB_SIZE - icon_size());
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
    painter.fill_rectangle_rounded(value_bound(), 6, color(THEME_ACCENT));
    painter.draw_rectangle_rounded(track_bound(), 6, 1, color(THEME_ACCENT));
    painter.fill_rectangle_rounded(value_bound().take_right(16).shrinked(Insetsi{12, 6}), 6, color(THEME_FOREGROUND));
}

} // namespace Widget
