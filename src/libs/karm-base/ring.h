#pragma once

#include "manual.h"
#include "panic.h"

namespace Karm {

template <typename T>
struct Ring {
    Manual<T>* _buf{};
    usize _cap{};
    usize _len{};
    usize _head{};
    usize _tail{};

    constexpr Ring() = default;

    Ring(usize cap)
        : _cap(cap) {
        _buf = new Manual<T>[cap];
    }

    Ring(Ring const& other) {
        _cap = other._cap;
        _buf = new Manual<T>[_cap];
        for (usize i = 0; i < other._len; i++)
            pushBack(other.peek(i));
    }

    Ring(Ring&& other)
        : _buf(std::exchange(other._buf, nullptr)),
          _cap(std::exchange(other._cap, 0)),
          _len(std::exchange(other._len, 0)),
          _head(std::exchange(other._head, 0)),
          _tail(std::exchange(other._tail, 0)) {
    }

    ~Ring() {
        for (usize i = 0; i < _len; i++)
            _buf[(_tail + i) % _cap].dtor();
        delete[] _buf;
    }

    Ring& operator=(Ring const& other) {
        *this = Ring(other);
        return *this;
    }

    Ring& operator=(Ring&& other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
        std::swap(_head, other._head);
        std::swap(_tail, other._tail);
        return *this;
    }

    void pushBack(T value) {
        if (_len == _cap) [[unlikely]]
            panic("push on full ring");

        _buf[_head].ctor(value);
        _head = (_head + 1) % _cap;
        _len++;
    }

    T popBack() {
        if (_len == 0) [[unlikely]]
            panic("pop on empty ring");

        T value = _buf[_head].take();
        _head = (_head - 1) % _cap;
        _len--;
        return value;
    }

    T popFront() {
        if (_len == 0) [[unlikely]]
            panic("dequeue on empty ring");

        T value = _buf[_tail].take();
        _tail = (_tail + 1) % _cap;
        _len--;
        return value;
    }

    void clear() {
        for (usize i = 0; i < _len; i++)
            _buf[(_tail + i) % _cap].dtor();

        _head = 0;
        _tail = 0;
        _len = 0;
    }

    T& peek(usize index) {
        if (index >= _len) [[unlikely]]
            panic("peek out of bounds");

        return _buf[(_tail + index) % _cap].unwrap();
    }

    T const& peek(usize index) const {
        if (index >= _len) [[unlikely]]
            panic("peek out of bounds");

        return _buf[(_tail + index) % _cap].unwrap();
    }

    void trunc(usize newLen) {
        if (newLen > _len) [[unlikely]]
            panic("trunc to longer length");

        for (usize i = newLen; i < _len; i++)
            _buf[(_tail + i) % _cap].dtor();

        _len = newLen;
    }

    usize head() const {
        return _head;
    }

    usize tail() const {
        return _tail;
    }

    usize len() const {
        return _len;
    }

    usize cap() const {
        return _cap;
    }

    usize rem() const {
        return _cap - _len;
    }
};

} // namespace Karm
