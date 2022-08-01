#pragma once

#include "component.h"
#include "karm-base/func.h"

namespace Karm::Ui {

template <typename T>
struct StateComponent : public Component<StateComponent<T>> {
    struct Accessor {
        StateComponent<T> &_s;

        Accessor(StateComponent<T> &s) : _s(s) {}

        void update(T t) {
            _s._value = t;
            _s.shouldRebuild();
        }

        T const &value() const {
            return _s._value;
        }

        Func<void()> bind(auto f) {
            return [*this, f] mutable {
                update(f(value()));
            };
        }

        void dispatch(auto f) { update(f(value())); }
    };

    T _value;
    Func<Child(Accessor)> _build;

    StateComponent(T initial, Func<Child(Accessor)> build)
        : _value(initial), _build(std::move(build)) {}

    Child build() override {
        return _build(Accessor(*this));
    }
};

template <typename T>
using State = typename StateComponent<T>::Accessor;

template <typename T>
Child state(T initial, Func<Child(State<T>)> build) {
    return makeStrong<StateComponent<T>>(initial, std::move(build));
}

} // namespace Karm::Ui
