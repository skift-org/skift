#pragma once

#include "_prelude.h"

#include "cons.h"
#include "iter.h"
#include "opt.h"
#include "rc.h"

namespace Karm {

template <typename _T>
struct List {
    using T = _T;

    struct Node {
        T buf;
        Strong<Node> next;
        Weak<Node> prev;
    };

    size_t _len = 0;
    OptStrong<Node> _head = NONE;
    OptWeak<Node> _tail = NONE;

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

    /* --- Capacity --- */

    void truncate(size_t len) {
        if (len > _len) {
            return;
        }

        if (len == 0) {
            _head = NONE;
            _tail = NONE;
            _len = 0;
            return;
        }

        auto node = _head.unwrap();
        for (size_t i = 0; i < len - 1; i++) {
            node = node->next.unwrap();
        }

        node->next = NONE;
        _tail = node;
        _len = len;
    }

    void clear() {
        _head = NONE;
        _tail = NONE;
    }

    /* --- Random Access --- */

    template <typename Self>
    constexpr auto _at(Self *self, size_t index) {
        if (index >= self->_len) {
            panic("index out of range");
        }

        Strong<Node> node = self->_head;
        for (size_t i = 0; i < index; i++) {
            node = node->next;
        }

        return node->buf;
    }

    T &at(size_t index) { return _at(this, index); }

    T const &at(size_t index) const { return _at(this, index); }

    T &operator[](size_t i) { return at(i); }

    T const &operator[](size_t i) const { return at(i); }

    /* --- Back Access --- */

    T &peekBack() {
        if (not _len) {
            panic("peek from empty list");
        }

        return _tail->buf;
    }

    T const &peekBack() const {
        if (not _len) {
            panic("peek from empty list");
        }

        return _tail->buf;
    }

    void pushBack(T const &value) {
        pushBack(T{value});
    }

    void pushBack(T &&value) {
        emplaceBack(std::move(value));
    }

    template <typename... Args>
    void emplaceBack(Args &&...args) {
        Strong<Node> node = makeStrong<Node>(std::forward(args)..., NONE, NONE);

        if (_tail) {
            _tail->next = node;
            node->prev = _tail;
        } else {
            _head = node;
        }

        _tail = node;
        _len++;
    }

    T popBack() {
        if (not _len) {
            panic("pop from empty list");
        }

        Strong<Node> node = _tail;
        _tail = node->prev;
        if (_tail) {
            _tail->next = NONE;
        } else {
            _head = NONE;
        }

        _len--;
        return node->buf;
    }

    /* --- Iteration --- */

    template <typename Self>
    static auto iter(Self *self) {
        return Iter([curr = self->_head]() mutable -> Opt<T> {
            if (curr) {
                auto ret = curr->buf;
                curr = curr->next;
                return ret;
            } else {
                return NONE;
            }
        });
    }

    template <typename Self>
    static auto iterRev(Self self) {
        return Iter([curr = self->_tail]() mutable -> Opt<T> {
            if (curr) {
                auto ret = curr->buf;
                curr = curr->prev;
                return ret;
            } else {
                return NONE;
            }
        });
    }

    constexpr auto iter() { return _iter(this); }

    constexpr auto iter() const { return _iter(this); }

    constexpr auto iterRev() { return _iterRev(this); }

    constexpr auto iterRev() const { return _iterRev(this); }

    /* --- Len & Buf --- */

    size_t len() const {
        return _len;
    }
};

} // namespace Karm
