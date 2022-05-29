#pragma once

#include "_prelude.h"

#include "array.h"
#include "clamp.h"
#include "inert.h"
#include "iter.h"
#include "opt.h"
#include "ref.h"

namespace Karm {

template <typename T>
struct Vec {
    Inert<T> *_buf = nullptr;
    size_t _cap = 0;
    size_t _len = 0;

    constexpr Vec() = default;

    Vec(size_t cap) : _cap(cap) {
        _buf = new Inert<T>[cap];
    }

    Vec(Vec const &other) {
        _cap = other._cap;
        _len = other._len;
        _buf = new Inert<T>[_cap];
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(other.peek(i));
        }
    }

    Vec(Vec &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
    }

    Vec(std::initializer_list<T> &&other) {
        _cap = other._len;
        _len = other._len;
        _buf = new Inert<T>[_cap];
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(std::move<T>(other.peek(i)));
        }
    }

    ~Vec() {
        for (size_t i = 0; i < _len; i++) {
            _buf[i].dtor();
        }

        delete[] _buf;
    }

    Vec &operator=(Vec const &other) {
        return *this = Vec(other);
    }

    Vec &operator=(Vec &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);

        return *this;
    }

    void ensure(size_t cap) {
        if (cap <= _cap)
            return;

        Inert<T> *tmp = new Inert<T>[cap];
        for (size_t i = 0; i < _len; i++) {
            tmp[i].ctor(_buf[i].take());
        }

        delete[] _buf;
        _buf = tmp;
        _cap = cap;
    }

    void insert(size_t index, T const &value) {
        insert(index, T(value));
    }

    void insert(size_t index, T &&value) {
        ensure(_len + 1);
        for (size_t i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }
        _buf[index].ctor(std::forward<T>(value));
        _len++;
    }

    T remove(size_t index) {
        T tmp = _buf[index].take();
        for (size_t i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return tmp;
    }

    void truncate(size_t len) {
        len = min(len, _len);

        for (size_t i = len; i < _len; i++) {
            _buf[i].dtor();
        }

        _len = len;
    }

    void clear() {
        for (size_t i = 0; i < _len; i++) {
            _buf[i].dtor();
        }
        _len = 0;
    }

    void push(T const &value) {
        insert(_len, value);
    }

    void push(T &&value) {
        insert(_len, std::forward<T>(value));
    }

    T pop() {
        if (_len == 0) {
            Debug::panic("pop on empty Vec");
        }

        return remove(_len - 1);
    }

    void put(size_t index, T const &value) {
        if (index >= _len) {
            Debug::panic("index out of range");
        }

        _buf[index].dtor();
        _buf[index].ctor(value);
    }

    T &top() {
        if (_len == 0) {
            Debug::panic("peek on empty Vec");
        }

        return _buf[_len - 1].unwrap();
    }

    T const &top() const {
        if (_len == 0) {
            Debug::panic("peek on empty Vec");
        }

        return _buf[_len - 1].unwrap();
    }

    T &peek(size_t index) {
        if (index >= _len) {
            Debug::panic("index out of range");
        }

        return _buf[index].unwrap();
    }

    T const &peek(size_t index) const {
        if (index >= _len) {
            Debug::panic("index out of range");
        }

        return _buf[index].unwrap();
    }

    constexpr auto iter() {
        return Iter([&, i = 0uz]() mutable -> T * {
            if (i >= _len) {
                return nullptr;
            }

            return &_buf[i++].unwrap();
        });
    }

    constexpr auto iter() const {
        return Iter([&, i = 0uz]() mutable -> T const * {
            if (i >= _len) {
                return nullptr;
            }

            return &_buf[i++].unwrap();
        });
    }

    constexpr auto iter_rev() {
        return Iter([&, i = _len]() mutable -> T * {
            if (i == 0) {
                return NONE;
            }

            i--;
            return &_buf[i].unwrap();
        });
    }

    constexpr auto iter_rev() const {
        return Iter([&, i = _len]() mutable -> T const * {
            if (i == 0) {
                return NONE;
            }

            i--;
            return &_buf[i].unwrap();
        });
    }

    T *buf() {
        return &_buf[0].unwrap();
    }

    T const *buf() const { return &_buf[0].unwrap(); }

    size_t len() const { return _len; }
};

template <typename T, size_t CAP>
struct InlineVec {
    Array<Inert<T>, CAP> _buf;
    size_t _len;

    InlineVec() : _len(0) {}

    InlineVec(InlineVec const &other) {
        _len = other._len;
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(other.peek(i));
        }
    }

    InlineVec(InlineVec &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
    }

    InlineVec(std::initializer_list<T> &&other) {
        _len = other._len;
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(std::move<T>(other.peek(i)));
        }
    }

    InlineVec &operator=(InlineVec const &other) {
        return *this = InlineVec(other);
    }

    InlineVec &operator=(InlineVec &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);

        return *this;
    }

    void ensure(size_t cap) {
        if (cap <= CAP)
            return;

        Debug::panic("InlineVec: cap too large");
    }

    void insert(size_t index, T const &value) {
        insert(index, T(value));
    }

    void insert(size_t index, T &&value) {
        ensure(_len + 1);
        for (size_t i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }
        _buf[index].ctor(std::forward<T>(value));
        _len++;
    }

    T remove(size_t index) {
        T tmp = _buf[index].take();
        for (size_t i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return tmp;
    }

    void truncate(size_t len) {
        len = min(len, _len);

        for (size_t i = len; i < _len; i++) {
            _buf[i].dtor();
        }

        _len = len;
    }

    void clear() {
        for (size_t i = 0; i < _len; i++) {
            _buf[i].dtor();
        }
        _len = 0;
    }

    void push(T const &value) {
        insert(_len, value);
    }

    void push(T &&value) {
        insert(_len, std::forward<T>(value));
    }

    T pop() {
        if (_len == 0) {
            Debug::panic("pop on empty Vec");
        }

        return remove(_len - 1);
    }

    void put(size_t index, T const &value) {
        if (index >= _len) {
            Debug::panic("index out of range");
        }

        _buf[index].dtor();
        _buf[index].ctor(value);
    }

    T &top() {
        if (_len == 0) {
            Debug::panic("peek on empty Vec");
        }

        return _buf[_len - 1].unwrap();
    }

    T const &top() const {
        if (_len == 0) {
            Debug::panic("peek on empty Vec");
        }

        return _buf[_len - 1].unwrap();
    }

    T &peek(size_t index) {
        if (index >= _len) {
            Debug::panic("index out of range");
        }

        return _buf[index].unwrap();
    }

    T const &peek(size_t index) const {
        if (index >= _len) {
            Debug::panic("index out of range");
        }

        return _buf[index].unwrap();
    }

    constexpr auto iter() {
        return Iter([&, i = 0uz]() mutable -> T * {
            if (i >= _len) {
                return nullptr;
            }

            return &_buf[i++].unwrap();
        });
    }

    constexpr auto iter() const {
        return Iter([&, i = 0uz]() mutable -> T const * {
            if (i >= _len) {
                return nullptr;
            }

            return &_buf[i++].unwrap();
        });
    }

    constexpr auto iter_rev() {
        return Iter([&, i = _len]() mutable -> T * {
            if (i == 0) {
                return NONE;
            }

            i--;
            return &_buf[i].unwrap();
        });
    }

    constexpr auto iter_rev() const {
        return Iter([&, i = _len]() mutable -> T const * {
            if (i == 0) {
                return NONE;
            }

            i--;
            return &_buf[i].unwrap();
        });
    }

    T *buf() {
        return &_buf[0].unwrap();
    }

    T const *buf() const { return &_buf[0].unwrap(); }

    size_t len() const { return _len; }
};

} // namespace Karm
