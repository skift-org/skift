#pragma once

#include <libgraphic/Painter.h>
#include <libwidget/Widget.h>

class Switch : public Widget
{
private:
    bool _state;

public:
    Switch(Widget *parent) : Widget(parent)
    {
        pin_width(36);
        pin_height(18);
    }

    ~Switch()
    {
    }

    virtual void paint(Painter &painter, const Recti &)
    {
        auto c = _state ? color(THEME_ACCENT) : color(THEME_FOREGROUND_DISABLED);

        painter.draw_rectangle_rounded(bound(), 99, 2, c);

        auto content = bound().shrinked(4);
        if (_state)
        {
            painter.fill_rectangle_rounded(content.take_right(content.height()), 99, c);
        }
        else
        {
            painter.fill_rectangle_rounded(content.take_left(content.height()), 99, c);
        }
    }

    virtual void event(Event *event)
    {
        if (event->type == Event::MOUSE_BUTTON_PRESS)
        {
            _state = !_state;
            should_repaint();
            event->accepted = true;

            Event event_value_changed = {};
            event_value_changed.type = Event::VALUE_CHANGE;
            dispatch_event(&event_value_changed);
        }
    }
};