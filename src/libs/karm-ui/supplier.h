#pragma once

#include "proxy.h"

namespace Karm::Ui {

template <typename I, typename U = I>
struct Supplier : public Proxy<Supplier<I>> {
    U _provided;

    using Node::parent;

    template <typename... Args>
    Supplier(Child child, Args... args)
        : Proxy<Supplier<I>>(child),
          _provided(std::forward<Args>(args)...) {
    }

    void *query(Meta::Id id) override {
        if (id == Meta::makeId<I>()) {
            return static_cast<I *>(&this->_provided);
        }

        Node *p = parent();
        return p ? p->query(id) : nullptr;
    }
};

template <typename I, typename U = I, typename... Args>
static inline Child supplier(Child child, Args &&...provided) {
    return makeStrong<Supplier<I>>(child, std::forward<Args>(provided)...);
}

} // namespace Karm::Ui
