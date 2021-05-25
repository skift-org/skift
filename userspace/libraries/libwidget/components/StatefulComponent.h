#pragma once

#include <libwidget/components/RebuildableComponent.h>

namespace Widget
{

template <typename TState>
struct StatefulComponentBase :
    public RebuildableComponent
{
private:
    TState _state{};

public:
    TState const &state() const
    {
        return _state;
    }

    void update(TState state)
    {
        if (_state != state)
        {
            _state = state;
            should_rebuild();
        }
    }

    StatefulComponentBase()
        : _state{}
    {
    }

    StatefulComponentBase(TState state)
        : _state{state}
    {
    }
};

template <
    typename TState,
    typename TStatefulBase = StatefulComponentBase<TState>>
struct StatefulComponent : public TStatefulBase
{
    using TStatefulBase::state;
    using TStatefulBase::update;

    StatefulComponent()
        : TStatefulBase()
    {
    }

    StatefulComponent(TState state)
        : TStatefulBase(state)
    {
    }

    RefPtr<Element> do_build() final
    {
        return build(state());
    }

    virtual RefPtr<Element> build(TState)
    {
        return nullptr;
    }
};

template <
    typename TState,
    typename TCallback,
    typename TStatefulBase = StatefulComponentBase<TState>>
struct FunctionalStatefulComponent : public TStatefulBase
{
private:
    TCallback _callback;

public:
    using TStatefulBase::state;
    using TStatefulBase::update;

    FunctionalStatefulComponent(TState state, TCallback callback)
        : TStatefulBase(state), _callback{callback}
    {
    }

    RefPtr<Element> do_build() final
    {
        return _callback(state(), [this](TState state) {
            update(state);
        });
    }
};

template <
    typename TState,
    typename TCallback,
    typename TStateful = FunctionalStatefulComponent<TState, TCallback>>
static inline RefPtr<TStateful> stateful(
    TCallback callback)
{
    return make<TStateful>(TState{}, callback);
}

template <
    typename TState,
    typename TCallback,
    typename TStateful = FunctionalStatefulComponent<TState, TCallback>>
static inline RefPtr<TStateful> stateful(
    TState state, TCallback callback)
{
    return make<TStateful>(state, callback);
}

} // namespace Widget
