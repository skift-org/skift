#pragma once

#include "hook.h"

namespace Karm::Ui {

struct _AtomHook : public Hook {
};

template <typename T>
struct Atom {
    T _value;

    T const &value() const {
        return _value;
    }

    void update(auto fn) {
        _value = fn(_value);
    }
};

template <typename T>
Atom<T> useAtom() {
    return {};
}

} // namespace Karm::Ui
