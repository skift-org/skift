#pragma once

#include "iter.h"
#include "opt.h"
#include "rc.h"
#include "std.h"

namespace Karm {

template <typename _T>
struct List {
    using T = _T;

    struct Node {
        T buf;
        OptStrong<Node> next;
        OptWeak<Node> prev;

        template <typename... Args>
        constexpr Node(Args &&...args)
            : buf{std::forward<Args>(args)...},
              next(NONE),
              prev(NONE) {}
    };

    usize _len = 0;
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

    void truncate(usize len) {
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
        for (usize i = 0; i < len - 1; i++) {
            node = node->next.unwrap();
        }

        node->next = NONE;
        _tail = node;
        _len = len;
    }

    void remove(T const &elem) {
        auto node = _head.unwrap();
        for (usize i = 0; i < _len; i++) {
            if (node->buf == elem) {
                if (i == 0) {
                    _head = node->next;
                } else {
                    node->prev.unwrap()->next = node->next;
                }

                if (i == _len - 1) {
                    _tail = node->prev;
                } else {
                    node->next.unwrap()->prev = node->prev;
                }

                _len--;
                return;
            }

            node = node->next.unwrap();
        }
    }

    void clear() {
        _head = NONE;
        _tail = NONE;
    }

    /* --- Random Access --- */

    template <typename Self>
    constexpr auto _at(Self *self, usize index) {
        if (index >= self->_len) {
            panic("index out of range");
        }

        Strong<Node> node = self->_head;
        for (usize i = 0; i < index; i++) {
            node = node->next;
        }

        return node->buf;
    }

    T &at(usize index) { return _at(this, index); }

    T const &at(usize index) const { return _at(this, index); }

    T &operator[](usize i) { return at(i); }

    T const &operator[](usize i) const { return at(i); }

    /* -- Front Access --- */

    T &peekFront() {
        if (not _len) {
            panic("peek from empty list");
        }

        return _head->buf;
    }

    T const &peekFront() const {
        if (not _len) {
            panic("peek from empty list");
        }

        return _head->buf;
    }

    void pushFront(T const &value) {
        pushFront(T{value});
    }

    void pushFront(T &&value) {
        emplaceFront(std::move(value));
    }

    template <typename... Args>
    void emplaceFront(Args &&...args) {
        Strong<Node> node = makeStrong<Node>(std::forward<Args>(args)...);

        if (_head) {
            _head->prev = node;
            node->next = _head;
        } else {
            _tail = node;
        }

        _head = node;
        _len++;
    }

    T popFront() {
        if (not _len) {
            panic("pop from empty list");
        }

        auto node = _head;
        _head = node->next;
        _len--;

        if (not _len) {
            _tail = NONE;
        }

        return std::move(node->buf);
    }

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
        Strong<Node> node = makeStrong<Node>(std::forward<Args>(args)...);

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
        return std::move(node->buf);
    }

    /* --- Queue --- */

    void requeue() {
        if (_len < 2) {
            return;
        }

        auto node = _head;
        _head = node->next;
        _tail->next = node;
        node->next = NONE;
        node->prev = _tail;
        _tail = node;
    }

    /* --- Iteration --- */

    template <typename Self>
    static auto _iter(Self *self) {
        return Iter([curr = self->_head]() mutable -> T * {
            if (curr) {
                auto &ret = curr->buf;
                curr = curr->next;
                return &ret;
            }
            return nullptr;
        });
    }

    template <typename Self>
    static auto _iterRev(Self *self) {
        return Iter([curr = self->_tail]() mutable -> T * {
            if (curr) {
                auto &ret = curr->buf;
                curr = curr->prev;
                return &ret;
            }
            return nullptr;
        });
    }

    constexpr auto iter() {
        return _iter(this);
    }

    constexpr auto iter() const {
        return _iter(this);
    }

    constexpr auto iterRev() {
        return _iterRev(this);
    }

    constexpr auto iterRev() const {
        return _iterRev(this);
    }

    /* --- Len & Buf --- */

    usize len() const {
        return _len;
    }
};

} // namespace Karm
