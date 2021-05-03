#pragma once

#include <libwidget/components/Rebuildable.h>

namespace Widget
{

template <typename TState>
class Stateful : public Rebuildable
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

template <typename TState, typename TCallback>
struct FonctionalStateful : public Rebuildable
{
private:
    TState _state{};
    TCallback _callback;

public:
    FonctionalStateful(TCallback callback)
        : _callback{callback}
    {
    }

    void update(TState state)
    {
        _state = state;
        should_rebuild();
    }

    RefPtr<Element> do_build() final
    {
        return _callback(_state, [this](TState state) { update(state); });
    }
};

template <typename TState, typename TCallback>
static inline RefPtr<FonctionalStateful<TState, TCallback>> stateful(TCallback callback)
{
    return make<FonctionalStateful<TState, TCallback>>(callback);
}

} // namespace Widget
