#pragma once

#include "hook.h"

namespace Karm::Ui {

struct _AtomHook : public Hook {
};

template <typename T>
struct Atom {
    T _value;

    auto value() const -> T const & {
        return _value;
    }

    void update(auto fn) {
        _value = fn(_value);
    }
};

template <typename T>
Atom<T> use_atom() {
    return {};
}

} // namespace Karm::Ui
