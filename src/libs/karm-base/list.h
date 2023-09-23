#pragma once

#include "cons.h"
#include "iter.h"
#include "opt.h"
#include "rc.h"
#include "std.h"

namespace Karm {

template <typename T>
struct List {
    struct Node {
        T buf;
        Node *next = nullptr;
        Node *prev = nullptr;

        template <typename... Args>
        constexpr Node(Args &&...args)
            : buf{std::forward<Args>(args)...} {}

        Cons<Node *, Node *> clone() const {
            auto *n = new Node(buf);

            if (next) {
                auto [head, tail] = next->clone();
                n->next = head;
                n->next->prev = n;
                return {n, tail};
            }
            return {n, n};
        }

        void remove() {
            if (prev) {
                prev->next = next;
            }

            if (next) {
                next->prev = prev;
            }

            delete this;
        }
    };

    usize _len = 0;
    Node *_head = nullptr;
    Node *_tail = nullptr;

    constexpr List() = default;

    List(List const &other) {
        if (other._len == 0) {
            return;
        }

        auto [head, tail] = other._head->clone();
        _head = head;
        _tail = tail;
        _len = other._len;
    }

    List(List &&other) {
        std::swap(_len, other._len);
        std::swap(_head, other._head);
        std::swap(_tail, other._tail);
    }

    List &operator=(List const &other) {
        *this = List(other);
        return *this;
    }

    List &operator=(List &&other) {
        std::swap(_len, other._len);
        std::swap(_head, other._head);
        std::swap(_tail, other._tail);
        return *this;
    }

    ~List() {
        clear();
    }

    /* --- Indexing --- */

    Node *_at(usize i) const {
        if (i >= _len) {
            panic("index out of range");
        }

        if (i < _len / 2) {
            auto n = _head;
            for (usize j = 0; j < i; j++) {
                n = n->next;
            }
            return n;
        } else {
            auto n = _tail;
            for (usize j = _len - 1; j > i; j--) {
                n = n->prev;
            }
            return n;
        }
    }

    void insert(usize i, T el) {
        emplace(i, el);
    }

    template <typename... Args>
    void emplace(usize i, Args &&...args) {
        auto *node = new Node(std::forward<Args>(args)...);

        if (i == 0) {
            if (_head) {
                _head->prev = node;
            } else {
                _tail = node;
            }
            node->next = _head;
            _head = node;
        } else if (i == _len) {
            if (_tail) {
                _tail->next = node;
            } else {
                _head = node;
            }
            node->prev = _tail;
            _tail = node;
        } else {
            auto *n = _at(i);
            node->next = n;
            node->prev = n->prev;
            n->prev = node;
            node->prev->next = node;
        }

        _len++;
    }

    void removeAt(usize i) {
        if (i >= _len) {
            panic("index out of range");
        }

        if (i == 0) {
            auto *n = _head;
            _head = _head->next;
            if (_head) {
                _head->prev = nullptr;
            } else {
                _tail = nullptr;
            }
            delete n;
        } else if (i == _len - 1) {
            auto *n = _tail;
            _tail = _tail->prev;
            if (_tail) {
                _tail->next = nullptr;
            } else {
                _head = nullptr;
            }
            delete n;
        } else {
            auto *n = _at(i);
            n->prev->next = n->next;
            n->next->prev = n->prev;
            delete n;
        }

        _len--;
    }

    void remove(T const &elem) {
        if (_len == 0) {
            return;
        }

        auto *n = _head;
        while (n) {
            if (n->buf == elem) {
                if (n == _head) {
                    _head = _head->next;
                    if (_head) {
                        _head->prev = nullptr;
                    } else {
                        _tail = nullptr;
                    }
                } else if (n == _tail) {
                    _tail = _tail->prev;
                    if (_tail) {
                        _tail->next = nullptr;
                    } else {
                        _head = nullptr;
                    }
                } else {
                    n->prev->next = n->next;
                    n->next->prev = n->prev;
                }

                auto *next = n->next;
                delete n;
                n = next;
                _len--;
            } else {
                n = n->next;
            }
        }
    }

    void truncate(usize len) {
        if (len >= _len) {
            return;
        }
        if (len == 0) {
            clear();
            return;
        }

        auto *n = _at(len);
        _tail = n->prev;
        _tail->next = nullptr;

        while (n) {
            auto *next = n->next;
            delete n;
            n = next;
        }

        _len = len;
    }

    T &at(usize i) {
        if (i >= _len)
            panic("index out of range");
        return _at(i)->buf;
    }

    T const &at(usize i) const {
        if (i >= _len)
            panic("index out of range");
        return _at(i)->buf;
    }

    void clear() {
        while (_head) {
            auto *next = _head->next;
            delete _head;
            _head = next;
        }
        _tail = nullptr;
        _len = 0;
    }

    /* --- Random Access --- */

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
        emplace(0, std::forward<Args>(args)...);
    }

    T popFront() {
        auto *n = _at(0);
        T buf = std::move(n->buf);
        removeAt(0);
        return buf;
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
        emplace(_len, std::forward<Args>(args)...);
    }

    T popBack() {
        auto *n = _at(_len - 1);
        T buf = std::move(n->buf);
        removeAt(_len - 1);
        return buf;
    }

    /* --- Queue --- */

    void requeue() {
        if (_len < 2) {
            return;
        }

        auto *n = _head;
        _head = _head->next;
        _tail->next = n;
        n->prev = _tail;
        n->next = nullptr;
        _tail = n;
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
