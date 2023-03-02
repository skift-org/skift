#pragma once

#include "inert.h"
#include "iter.h"
#include "opt.h"

namespace Karm {

template <typename T>
struct Ring {
    Inert<T> *_buf{};
    usize _cap{};
    usize _len{};
    usize _head{};
    usize _tail{};

    constexpr Ring() = default;

    Ring(usize cap) : _cap(cap) {
        _buf = new Inert<T>[cap];
    }

    Ring(Ring const &other) {
        _cap = other._cap;
        _len = other._len;
        _head = other._head;
        _tail = other._tail;
        _buf = new Inert<T>[_cap];
        for (usize i = 0; i < _len; i++) {
            _buf[i].ctor(other.peek(i));
        }
    }

    Ring(Ring &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
        std::swap(_head, other._head);
        std::swap(_tail, other._tail);
    }

    ~Ring() {
        for (usize i = 0; i < _len; i++) {
            _buf[(_tail + i) % _cap].dtor();
        }

        delete[] _buf;
    }

    Ring &operator=(Ring const &other) {
        *this = Ring(other);
        return *this;
    }

    Ring &operator=(Ring &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
        std::swap(_head, other._head);
        std::swap(_tail, other._tail);
        return *this;
    }

    void pushBack(T value) {
        if (_len == _cap) {
            panic("push on full ring");
        }

        _buf[_head].ctor(value);
        _head = (_head + 1) % _cap;
        _len++;
    }

    T popBack() {
        if (_len == 0) {
            panic("pop on empty ring");
        }

        T value = _buf[_head].take();
        _head = (_head - 1) % _cap;
        _len--;
        return value;
    }

    T dequeue() {
        if (_len == 0) {
            panic("dequeue on empty ring");
        }

        T value = _buf[_tail].take();
        _tail = (_tail + 1) % _cap;
        _len--;
        return value;
    }

    void clear() {
        for (usize i = 0; i < _len; i++) {
            _buf[(_tail + i) % _cap].dtor();
        }

        _head = 0;
        _tail = 0;
        _len = 0;
    }

    T &peek(usize index) {
        if (index >= _len) {
            panic("peek out of bounds");
        }

        return _buf[(_tail + index) % _cap].unwrap();
    }

    T const &peek(usize index) const {
        if (index >= _len) {
            panic("peek out of bounds");
        }

        return _buf[(_tail + index) % _cap].unwrap();
    }

    usize len() const { return _len; }

    constexpr auto iter() {
        return Iter([&, i = 0uz]() mutable -> T * {
            if (i >= _len) {
                return nullptr;
            }

            return &_buf[(_tail + i) % _cap].unwrap();
        });
    }

    constexpr auto iter() const {
        return Iter([&, i = 0uz]() mutable -> T const * {
            if (i >= _len) {
                return nullptr;
            }

            return &_buf[(_tail + i) % _cap].unwrap();
        });
    }

    constexpr auto iterRev() {
        return Iter([&, i = _len]() mutable -> T * {
            if (i == 0) {
                return nullptr;
            }

            i--;
            return &_buf[(_tail + i) % _cap].unwrap();
        });
    }

    constexpr auto iterRev() const {
        return Iter([&, i = _len]() mutable -> T const * {
            if (i == 0) {
                return nullptr;
            }

            i--;
            return &_buf[(_tail + i) % _cap].unwrap();
        });
    }
};

} // namespace Karm
