#pragma once

#include <libwidget/Element.h>

namespace Widget
{

template <typename TState>
struct StatefulElement : public Element
{
private:
    TState _state = {};

public:
    Func<void(const TState &)> on_state_change;

    void state(TState state)
    {
        _state = state;

        if (on_state_change)
        {
            on_state_change(_state);
        }

        should_relayout();
        should_repaint();
    }

    const TState &state() const { return _state; }
};

} // namespace Widget
