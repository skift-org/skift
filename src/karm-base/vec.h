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

    auto operator=(Vec const &other) -> Vec & {
        return *this = Vec(other);
    }

    auto operator=(Vec &&other) -> Vec & {
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

    auto remove(size_t index) -> T {
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

    auto pop() -> Opt<T> {
        if (_len == 0)
            return NONE;

        return remove(_len - 1);
    }

    auto peek() -> OptRef<T> {
        if (_len == 0) {
            return NONE;
        }

        return Ref<T>(_buf[_len - 1]);
    }

    auto begin() -> T * {
        if (_len == 0) {
            return nullptr;
        }
        return &_buf[0].unwrap();
    }

    auto end() -> T * { return begin() + _len; }

    auto begin() const -> T const * { return &_buf[0].unwrap(); }

    auto end() const -> T const * { return begin() + _len; }

    auto buf() -> T * { return begin(); }

    auto buf() const -> T const * { return begin(); }

    auto len() const -> size_t { return _len; }
};

} // namespace Karm::Base
