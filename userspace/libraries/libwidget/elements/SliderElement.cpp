#include <libgraphic/Painter.h>
#include <libwidget/elements/SliderElement.h>

namespace Widget
{

int SliderElement::icon_size()
{
    return bound().height() - 8;
}

Math::Recti SliderElement::value_bound()
{

    return bound().take_left((bound().width() - THUMB_SIZE - (icon_size())) * state() + THUMB_SIZE + icon_size());
}

void SliderElement::slide_to(Math::Vec2i position)
{
    int pos = position.x() - bound().position().x() - THUMB_SIZE - icon_size() + THUMB_SIZE / 2;
    state(clamp(pos / (double)(bound().width() - THUMB_SIZE - icon_size()), 0, 1));

    Event event_value_changed = {};
    event_value_changed.type = Event::VALUE_CHANGE;
    dispatch_event(&event_value_changed);
}

SliderElement::SliderElement()
{
    min_width(160);
}

void SliderElement::event(Event *event)
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

void SliderElement::paint(Graphic::Painter &painter, const Math::Recti &)
{
    painter.fill_rectangle_rounded(value_bound(), 6, color(THEME_ACCENT));
    painter.draw_rectangle_rounded(bound(), 6, 1, color(THEME_ACCENT));
    painter.fill_rectangle_rounded(value_bound().take_right(16).shrinked(Insetsi{12, 6}), 6, color(THEME_FOREGROUND));
}

} // namespace Widget
