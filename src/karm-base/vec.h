#pragma once

#include "_prelude.h"

#include "inert.h"
#include "opt.h"
#include "ref.h"

namespace Karm::Base {

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
            _buf[i].ctor(other.unwrap[i]);
        }
    }

    Vec(Vec &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
    }

    Vec(std::initializer_list<T> &&list) {
        _cap = list.len();
        _len = _cap;
        _buf = new Inert<T>[_cap];
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(std::move<T>(list[i]));
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
            _buf[i].construct(_buf[i + 1].take());
        }
        _len--;
        return tmp;
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

    Opt<T> pop() {
        if (_len == 0)
            return NONE;

        return remove(_len - 1);
    }

    OptRef<T> peek() {
        if (_len == 0) {
            return NONE;
        }

        return Ref<T>(_buf[_len - 1]);
    }

    T *begin() {
        if (_len == 0) {
            return nullptr;
        }

        return &_buf[0].unwrap();
    }

    T *end() {
        return begin() + _len;
    }

    T const *begin() const {
        return &_buf[0].unwrap();
    }

    T const *end() const {
        return begin() + _len;
    }

    T *buf() {
        return begin();
    }

    size_t len() const {
        return _len;
    }
};

} // namespace Karm::Base
