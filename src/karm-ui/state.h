#pragma once

#include "hook.h"

namespace Karm::Ui {

struct _StateHook : public Hook {
};

template <typename T>
struct State {
    T _value;

    State(T value) : _value(value) {
    }

    auto value() const -> T const & {
        return _value;
    }

    void update(auto fn) {
        _value = fn(_value);
    }
};

template <typename T>
State<T> use_state(T value) {
    State<T> state{value};

    return state;
}

} // namespace Karm::Ui
