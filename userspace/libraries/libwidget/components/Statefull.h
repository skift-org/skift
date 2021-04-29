#pragma once

#include <libwidget/components/Rebuildable.h>

namespace Widget
{

template <typename TState>
class Statefull : public Rebuildable
{
private:
    TState _state{};

public:
    void update(TState state)
    {
        _state = state;
        should_rebuild();
    }

    RefPtr<Element> do_build() final { return build(_state); }
    virtual RefPtr<Element> build(TState) { return nullptr; }
};

} // namespace Widget
