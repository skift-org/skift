#include <libgraphic/Painter.h>
#include <libwidget/elements/ToggleElement.h>

namespace Widget
{

ToggleElement::ToggleElement()
{
    pin_width(WIDTH);
    pin_height(HEIGHT);
}

void ToggleElement::paint(Graphic::Painter &painter, const Math::Recti &)
{
    auto c = state() ? color(THEME_ACCENT) : color(THEME_FOREGROUND_DISABLED);

    auto control = Math::Recti{WIDTH, HEIGHT}.centered_within(bound());

    painter.draw_rectangle_rounded(control, 99, 2, c);

    auto content = control.shrinked(4);
    if (state())
    {
        painter.fill_rectangle_rounded(content.take_right(content.height()), 99, c);
    }
    else
    {
        painter.fill_rectangle_rounded(content.take_left(content.height()), 99, c);
    }
}

void ToggleElement::event(Event *event)
{
    if (event->type == Event::MOUSE_BUTTON_PRESS)
    {
        state(!state());
        should_repaint();
        event->accepted = true;

        Event event_value_changed = {};
        event_value_changed.type = Event::VALUE_CHANGE;
        dispatch_event(&event_value_changed);
    }
}

} // namespace Widget
