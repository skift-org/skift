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

    List &operator=(List const &other) = delete;

    List &operator=(List &&other) {
        std::swap(_len, other._len);
        std::swap(_head, other._head);
        std::swap(_tail, other._tail);
        return *this;
    }

    void push(T &&value) {
        Strong<_Node> node = makeStrong<_Node>(std::forward(value), NONE, NONE);

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

    T pop() {
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

    T &peek() {
        if (!_len) {
            Debug::panic("peek from empty list");
        }

        return _tail->buf;
    }

    T &peek(size_t index) {
        if (index >= _len) {
            Debug::panic("index out of range");
        }

        Strong<_Node> node = _head;
        for (size_t i = 0; i < index; i++) {
            node = node->next;
        }

        return node->buf;
    }

    T const &peek(size_t index) const {
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

        T &operator*() { return curr->buf; }

        Iter &operator++() {
            curr = curr->next;
            return *this;
        }

        bool operator!=(Iter const &other) const {
            return curr != other.curr;
        }
    };

    Iter begin() {
        return _head;
    }

    Iter end() {
        return _tail;
    }

    void clear() {
        _head = NONE;
        _tail = NONE;
    }

    size_t len() {
        return _len;
    }
};

} // namespace Karm::Base
