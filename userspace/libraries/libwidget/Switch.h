#pragma once

#include <libgraphic/Painter.h>
#include <libwidget/Container.h>
#include <libwidget/Element.h>
#include <libwidget/Label.h>

namespace Widget
{

class Switch : public Element
{
private:
    bool _state;

public:
    static constexpr auto WIDTH = 36;
    static constexpr auto HEIGHT = 18;

    bool state()
    {
        return _state;
    }

    void state(bool state)
    {
        _state = state;
        should_repaint();
    }

    Switch(bool state = false)
    {
        _state = state;
        pin_width(WIDTH);
        pin_height(HEIGHT);
    }

    ~Switch()
    {
    }

    virtual void paint(Graphic::Painter &painter, const Math::Recti &)
    {
        auto c = _state ? color(THEME_ACCENT) : color(THEME_FOREGROUND_DISABLED);

        auto control = Math::Recti{WIDTH, HEIGHT}.centered_within(bound());

        painter.draw_rectangle_rounded(control, 99, 2, c);

        auto content = control.shrinked(4);
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

static inline RefPtr<Switch> toggle(bool state = false) { return make<Switch>(state); }

} // namespace Widget
