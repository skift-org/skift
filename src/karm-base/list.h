#pragma once

#include "_prelude.h"

#include "cons.h"
#include "opt.h"
#include "rc.h"

namespace Karm::Base {

template <typename T>
struct List {
    struct _Node {
        T buf;
        Strong<_Node> next;
        Weak<_Node> prev;
    };

    size_t _len = 0;
    OptStrong<_Node> _head = NONE;
    OptWeak<_Node> _tail = NONE;

    constexpr List() = default;

    List(List const &other) = delete;

    List(List &&other) {
        std::swap(_len, other._len);
        std::swap(_head, other._head);
        std::swap(_tail, other._tail);
    }

    auto operator=(List const &other) -> List & = delete;

    auto operator=(List &&other) -> List & {
        std::swap(_len, other._len);
        std::swap(_head, other._head);
        std::swap(_tail, other._tail);
        return *this;
    }

    void push(T &&value) {
        Strong<_Node> node = make_strong<_Node>(std::forward(value), NONE, NONE);

        if (_tail) {
            _tail->next = node;
            node->prev = _tail;
        } else {
            _head = node;
        }

        _tail = node;
        _len++;
    }

    void push(T const &value) {
        push(T{value});
    }

    auto pop() -> T {
        if (!_len) {
            Debug::panic("pop from empty list");
        }

        Strong<_Node> node = _tail;
        _tail = node->prev;
        if (_tail) {
            _tail->next = NONE;
        } else {
            _head = NONE;
        }

        _len--;
        return node->buf;
    }

    auto peek() -> T & {
        if (!_len) {
            Debug::panic("peek from empty list");
        }

        return _tail->buf;
    }

    auto peek(size_t index) -> T & {
        if (index >= _len) {
            Debug::panic("index out of range");
        }

        Strong<_Node> node = _head;
        for (size_t i = 0; i < index; i++) {
            node = node->next;
        }

        return node->buf;
    }

    auto peek(size_t index) const -> T const & {
        if (index >= _len) {
            Debug::panic("index out of range");
        }

        Strong<_Node> node = _head;
        for (size_t i = 0; i < index; i++) {
            node = node->next;
        }

        return node->buf;
    }

    struct Iter {
        OptStrong<_Node> curr;

        operator bool() const { return curr; }

        auto operator*() -> T & { return curr->buf; }

        auto operator++() -> Iter & {
            curr = curr->next;
            return *this;
        }

        auto operator!=(Iter const &other) const -> bool {
            return curr != other.curr;
        }
    };

    auto begin() -> Iter {
        return _head;
    }

    auto end() -> Iter {
        return _tail;
    }

    void clear() {
        _head = NONE;
        _tail = NONE;
    }

    auto len() -> size_t {
        return _len;
    }
};

} // namespace Karm::Base
