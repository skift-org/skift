#pragma once

#include "_prelude.h"

#include "box.h"
#include "cons.h"
#include "opt.h"

namespace Karm::Base {

template <typename T>
struct List {
    struct Node : Cons<T, Opt<Box<Node>>> {
    };

    Opt<Node> _head = NONE;

    void push(T const &value) {
        _head = Node{value, _head};
    }

    void push(T &&value) {
        _head = Node{std::move(value), _head};
    }

    Opt<T> pop() {
        if (!_head) {
            return NONE;
        }

        auto value = _head->car;
        _head = _head->cdr;
        return value;
    }

    void clear() {
        _head = NONE;
    }

    auto begin() const {
        return _head;
    }

    auto end() const {
        return NONE;
    }

    size_t len() {
        size_t len = 0;
        for (auto const &i : *this) {
            len++;
        }
        return len;
    }
};

} // namespace Karm::Base
