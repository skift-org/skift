#pragma once

#include <libwidget/Component.h>

namespace Widget
{

template <typename TState>
class Statefull : public Component
{
private:
    TState _state = {};

public:
    const void state(TState state)
    {
        _state = state;
        should_rebuild();
    }

    const TState &state()
    {
        return _state;
    }
};

} // namespace Widget
