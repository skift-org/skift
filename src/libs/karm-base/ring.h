#pragma once

#include "inert.h"
#include "iter.h"
#include "opt.h"

namespace Karm {

template <typename T>
struct Ring {
    Inert<T> *_buf{};
    size_t _cap{};
    size_t _len{};
    size_t _head{};
    size_t _tail{};

    constexpr Ring() = default;

    Ring(size_t cap) : _cap(cap) {
        _buf = new Inert<T>[cap];
    }

    Ring(Ring const &other) {
        _cap = other._cap;
        _len = other._len;
        _head = other._head;
        _tail = other._tail;
        _buf = new Inert<T>[_cap];
        for (size_t i = 0; i < _len; i++) {
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
        for (size_t i = 0; i < _len; i++) {
            _buf[(_tail + i) % _cap].dtor();
        }

        delete[] _buf;
    }

    Ring &operator=(Ring const &other) {
        return *this = Ring(other);
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
        for (size_t i = 0; i < _len; i++) {
            _buf[(_tail + i) % _cap].dtor();
        }

        _head = 0;
        _tail = 0;
        _len = 0;
    }

    T &peek(size_t index) {
        if (index >= _len) {
            panic("peek on empty ring");
        }

        return _buf[(_tail + index) % _cap].unwrap();
    }

    T const &peek(size_t index) const {
        if (index >= _len) {
            panic("peek on empty ring");
        }

        return _buf[(_tail + index) % _cap].unwrap();
    }

    size_t len() const { return _len; }

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
