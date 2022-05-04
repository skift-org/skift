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

    T const &value() const {
        return _value;
    }

    void update(auto fn) {
        _value = fn(_value);
    }
};

template <typename T>
State<T> useState(T value) {
    State<T> state{value};

    return state;
}

} // namespace Karm::Ui
