#pragma once

#include <karm-meta/nocopy.h>

#include "iter.h"

namespace Karm {

template <typename T>
struct LlItem : Meta::Pinned {
    T* prev = nullptr;
    T* next = nullptr;

    operator bool() const {
        return prev or next;
    }
};

template <typename T, auto T::* Item = &T::item>
struct Ll {
    T* _head = nullptr;
    T* _tail = nullptr;
    usize _len = 0;

    Ll() = default;

    Ll(Ll&) = delete;

    Ll(Ll&& other) {
        _head = std::exchange(other._head, nullptr);
        _tail = std::exchange(other._tail, nullptr);
        _len = std::exchange(other._len, 0);
    }

    Ll& operator=(Ll&) = delete;

    Ll& operator=(Ll&& other) {
        _head = std::exchange(other._head, nullptr);
        _tail = std::exchange(other._tail, nullptr);
        _len = std::exchange(other._len, 0);
        return *this;
    }

    static auto& item(T* value) {
        return value->*Item;
    }

    static T*& prev(T* value) {
        return (value->*Item).prev;
    }

    static T*& next(T* value) {
        return (value->*Item).next;
    }

    T* head() { return _head; }

    T const* head() const { return _head; }

    T* tail() { return _tail; }

    T const* tail() const { return _tail; }

    usize len() const { return _len; }

    bool empty() const { return _len == 0; }

    T* detach(T* value) {
        if (not value) [[unlikely]]
            panic("value connot be null");

        if (prev(value))
            next(prev(value)) = next(value);

        if (next(value))
            prev(next(value)) = prev(value);

        if (_head == value)
            _head = next(value);

        if (_tail == value)
            _tail = prev(value);

        prev(value) = nullptr;
        next(value) = nullptr;

        _len--;

        return value;
    }

    T* append(T* value, T* after) {
        if (not value) [[unlikely]]
            panic("value connot be null");

        if (value == after) [[unlikely]]
            panic("cannot append a node to itself");

        if (after) {
            next(value) = next(after);
            prev(value) = after;

            if (next(after))
                prev(next(after)) = value;
            if (_tail == after)
                _tail = value;
            next(after) = value;
        } else {
            if (_tail) {
                next(_tail) = value;
                prev(value) = _tail;
                _tail = value;
            } else {
                _head = value;
                _tail = value;
            }
        }
        _len++;
        return value;
    }

    T* prepend(T* value, T* before) {
        if (before) {
            prev(value) = prev(before);
            next(value) = before;

            if (prev(before))
                next(prev(before)) = value;
            if (_head == before)
                _head = value;

            prev(before) = value;
        } else {
            if (_head) {
                prev(_head) = value;
                next(value) = _head;
                _head = value;
            } else {
                _head = value;
                _tail = value;
            }
        }
        _len++;
        return value;
    }

    void apply(auto&& f = [](T*) {
    }) {
        auto* node = _head;
        while (node) {
            f(node);
            node = next(node);
        }
    }

    void truncApply(
        usize len, auto&& f = [](T*) {
        }
    ) {
        while (_len > len)
            f(detach(_tail));
    }

    void clearApply(auto&& f = [](T*) {
    }) {
        while (_head)
            f(detach(_head));
    }

    T* peek(usize i) {
        if (i >= _len) [[unlikely]]
            panic("index out of bound");

        auto* node = _head;
        while (i--)
            node = next(node);
        return node;
    }

    T const* peek(usize i) const {
        if (i >= _len) [[unlikely]]
            panic("index out of bound");

        auto* node = _head;
        while (i--)
            node = next(node);
        return node;
    }

    bool requeue() {
        if (_len < 2)
            return false;

        auto* node = detach(_head);
        append(node, _tail);
        return true;
    }
};

template <typename T>
struct List {
    struct Item {
        T value;
        LlItem<Item> item{};
    };

    Ll<Item> _ll;

    List() = default;

    ~List() { clear(); }

    // MARK: Indexing

    void insert(usize i, T el) {
        auto* node = new Item{el};

        if (i == 0)
            _ll.prepend(node, nullptr);
        else
            _ll.append(node, _ll.peek(i - 1));
    }

    template <typename... Args>
    void emplace(usize i, Args&&... args) {
        auto* node = new Item{std::forward<Args>(args)...};

        if (i == 0)
            _ll.prepend(node, nullptr);
        else
            _ll.append(node, _ll.peek(i - 1));
    }

    void removeAt(usize i) {
        delete _ll.detach(_ll.peek(i));
    }

    void remove(T const& elem) {
        auto* node = _ll.head();
        while (node) {
            if (node->value == elem) {
                delete _ll.detach(node);
                return;
            }
            node = _ll.next(node);
        }
    }

    void trunc(usize len) {
        _ll.truncApply(len, [](Item* item) {
            delete item;
        });
    }

    void clear() {
        _ll.clearApply([](Item* item) {
            delete item;
        });
    }

    // MARK: Random Access

    T& peek(usize i) lifetimebound {
        return _ll.peek(i)->value;
    }

    T const& peek(usize i) const lifetimebound {
        return _ll.peek(i)->value;
    }

    T& operator[](usize i) lifetimebound {
        return peek(i);
    }

    T const& operator[](usize i) const lifetimebound {
        return peek(i);
    }

    // MARK: Front Access

    void pushFront(T const& value) {
        pushFront(T{value});
    }

    void pushFront(T&& value) {
        emplaceFront(std::move(value));
    }

    template <typename... Args>
    void emplaceFront(Args&&... args) {
        prepend(new Item{std::forward<Args>(args)...}, _ll.head());
    }

    T popFront() {
        auto* head = _ll.head();
        T buf = std::move(head->value);
        delete _ll.detach(head);
        return buf;
    }

    T& peekFront() lifetimebound {
        return _ll.head()->value;
    }

    T const& peekFront() const lifetimebound {
        return _ll.head()->value;
    }

    // MARK: Back Access

    void pushBack(T const& value) {
        pushBack(T{value});
    }

    void pushBack(T&& value) {
        emplaceBack(std::move(value));
    }

    template <typename... Args>
    void emplaceBack(Args&&... args) {
        _ll.append(new Item{std::forward<Args>(args)...}, _ll.tail());
    }

    T popBack() {
        auto* tail = _ll.tail();
        auto value = std::move(tail->value);
        delete _ll.detach(tail);
        return value;
    }

    T& peekBack() lifetimebound {
        return _ll.tail()->value;
    }

    // MARK: Queue

    bool requeue() {
        return _ll.requeue();
    }

    // MARK: Iteration

    template <typename Self>
    static auto _iter(Self* self) {
        return Iter([curr = self->_ll.head()] mutable -> T* {
            if (curr) {
                auto& ret = curr->value;
                curr = curr->item.next;
                return &ret;
            }
            return nullptr;
        });
    }

    template <typename Self>
    static auto _iterRev(Self* self) {
        return Iter([curr = self->_ll.tail()] mutable -> T* {
            if (curr) {
                auto& ret = curr->value;
                curr = curr->item.prev;
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

    // MARK: Len

    usize len() const {
        return _ll.len();
    }
};

} // namespace Karm
