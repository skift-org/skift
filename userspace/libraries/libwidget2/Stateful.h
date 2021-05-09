#pragma once

#include <libwidget2/Rebuildable.h>

namespace Widget2
{

template <typename TProps, typename TState>
struct BaseStateful : public Rebuildable<TProps>
{
    using StateType = TState;

private:
    TState _state{};

public:
    const TSate &state()
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
};

template <typename TProps, typename TState>
struct Stateful : public BaseStateful<TProps, TState>
{
    OwnPtr<BaseTransient> do_build() final
    {
        return build(props(), state());
    }

    virtual OwnPtr<BaseTransient> build(const TProps &, const TState &)
    {
        return nullptr;
    }
};

template <typename TCallback>
struct FunctionalStatefulProps
{
    TCallback callback;
};

template <typename TState, typename TCallback>
struct FonctionalStateful : public BaseStateful<FunctionalStatefulProps<TCallback>>
{
    RefPtr<BaseTransient> do_build() final
    {
        return props().callback(
            state(),
            [this](TState state) {
                update(state);
            });
    }
};

} // namespace Widget2
