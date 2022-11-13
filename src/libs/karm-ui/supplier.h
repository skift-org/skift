#pragma once

#include "node.h"

namespace Karm::Ui {

template <typename I, typename U = I>
struct Supplier : public ProxyNode<Supplier<I>> {
    U _provided;

    using Node::parent;

    template <typename... Args>
    Supplier(Child child, Args... args)
        : ProxyNode<Supplier<I>>(child),
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
inline Child supplier(Child child, Args &&...provided) {
    return makeStrong<Supplier<I>>(child, std::forward<Args>(provided)...);
}

} // namespace Karm::Ui
