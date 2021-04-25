#pragma once

#include <libwidget/Element.h>

namespace Widget
{

template <typename TState>
class StatefullElement : public Element
{
private:
    TState _state = {};

public:
    void state(TState state)
    {
        _state = state;
        should_relayout();
        should_repaint();
    }

    const TState &state() const { return _state; }
};

} // namespace Widget
