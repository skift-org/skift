#pragma once

#include "array.h"
#include "clamp.h"
#include "inert.h"

namespace Karm {

template <typename _T>
struct Buf {
    using T = _T;

    Inert<T> *_buf{};
    size_t _cap{};
    size_t _len{};

    Buf() = default;

    Buf(size_t cap) : _cap(cap) {
        _buf = new Inert<T>[cap];
    }

    Buf(std::initializer_list<T> other) {
        _cap = other.size();
        _len = other.size();
        _buf = new Inert<T>[_cap];
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(std::move(other.begin()[i]));
        }
    }

    Buf(Sliceable<T> auto &other) {
        _cap = other.len();
        _len = other.len();
        _buf = new Inert<T>[_cap];
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(other[i]);
        }
    }

    Buf(Buf const &other) {
        _cap = other._cap;
        _len = other._len;
        _buf = new Inert<T>[_cap];
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(other.at(i));
        }
    }

    Buf(Buf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
    }

    ~Buf() {
        for (size_t i = 0; i < _len; i++) {
            _buf[i].dtor();
        }

        delete[] _buf;
    }

    Buf &operator=(Buf const &other) {
        return *this = Buf(other);
    }

    Buf &operator=(Buf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
        return *this;
    }

    constexpr T &operator[](size_t i) { return _buf[i].unwrap(); }

    constexpr T const &operator[](size_t i) const { return _buf[i].unwrap(); }

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

    void fit() {
        if (_len == _cap)
            return;

        Inert<T> *tmp = new Inert<T>[_len];
        for (size_t i = 0; i < _len; i++) {
            tmp[i].ctor(_buf[i].take());
        }

        delete[] _buf;
        _buf = tmp;
        _cap = _len;
    }

    template <typename... Args>
    void emplace(size_t index, Args &&...args) {
        ensure(_len + 1);

        for (size_t i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }
        _buf[index].ctor(std::forward<Args>(args)...);
        _len++;
    }

    void insert(size_t index, T &&value) {
        ensure(_len + 1);

        for (size_t i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::move(value));
        _len++;
    }

    void insert(Copy, size_t index, T *first, size_t count) {
        ensure(_len + count);

        for (size_t i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - count].take());
        }

        for (size_t i = 0; i < count; i++) {
            _buf[index + i].ctor(first[i]);
        }

        _len += count;
    }

    void insert(Move, size_t index, T *first, size_t count) {
        ensure(_len + count);

        for (size_t i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - count].take());
        }

        for (size_t i = 0; i < count; i++) {
            _buf[index + i].ctor(std::move(first[i]));
        }

        _len += count;
    }

    T removeAt(size_t index) {
        if (index >= _len) {
            panic("index out of bounds");
        }

        T ret = _buf[index].take();
        for (size_t i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return ret;
    }

    void removeRange(size_t index, size_t count) {
        if (index > _len) {
            panic("index out of bounds");
        }

        if (index + count > _len) {
            panic("index + count out of bounds");
        }

        for (size_t i = index; i < _len - count; i++) {
            _buf[i].ctor(_buf[i + count].take());
        }

        _len -= count;
    }

    void resize(size_t newLen, T fill = {}) {
        if (newLen > _len) {
            ensure(newLen);
            for (size_t i = _len; i < newLen; i++) {
                _buf[i].ctor(fill);
            }
        } else if (newLen < _len) {
            for (size_t i = newLen; i < _len; i++) {
                _buf[i].dtor();
            }
        }
        _len = newLen;
    }

    void truncate(size_t newLen) {
        if (newLen >= _len)
            return;

        for (size_t i = newLen; i < _len; i++) {
            _buf[i].dtor();
        }

        _len = newLen;
    }

    T *take() {
        T *ret = &_buf->unwrap();
        _buf = nullptr;
        _cap = 0;
        _len = 0;
        return ret;
    }

    T *buf() {
        if (_buf == nullptr)
            return nullptr;

        return &_buf->unwrap();
    }

    T const *buf() const {
        if (_buf == nullptr)
            return nullptr;

        return &_buf->unwrap();
    }

    size_t len() const {
        return _len;
    }

    size_t cap() const {
        return _cap;
    }

    size_t size() const {
        return _len * sizeof(T);
    }
};

template <typename _T, size_t N>
struct InlineBuf {
    using T = _T;

    Array<Inert<T>, N> _buf = {};
    size_t _len = {};

    constexpr InlineBuf() = default;

    InlineBuf(size_t cap) {
        if (cap > N) {
            panic("cap too large");
        }
    }

    InlineBuf(std::initializer_list<T> other) {
        _len = other.size();
        if (_len > N) {
            panic("cap too large");
        }
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(std::move(other.begin()[i]));
        }
    }

    InlineBuf(Sliceable<T> auto &other) {
        _len = other.len();

        if (_len > N) {
            panic("cap too large");
        }

        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(other[i]);
        }
    }

    InlineBuf(InlineBuf const &other) {
        _len = other._len;
        if (_len > N) {
            panic("cap too large");
        }
        for (size_t i = 0; i < _len; i++) {
            _buf[i].ctor(other.at(i));
        }
    }

    InlineBuf(InlineBuf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
    }

    ~InlineBuf() = default;

    InlineBuf &operator=(InlineBuf const &other) {
        return *this = InlineBuf(other);
    }

    InlineBuf &operator=(InlineBuf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
        return *this;
    }

    constexpr T &operator[](size_t i) { return _buf[i].unwrap(); }

    constexpr T const &operator[](size_t i) const { return _buf[i]; }

    void ensure(size_t len) {
        if (len > N) {
            panic("cap too large");
        }
    }

    void fit() {
        // no-op
    }

    template <typename... Args>
    void emplace(size_t index, Args &&...args) {
        if (_len == N) {
            panic("cap too large");
        }

        for (size_t i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::forward<Args>(args)...);
        _len++;
    }

    void insert(size_t index, T &&value) {
        if (_len == N) {
            panic("cap too large");
        }

        for (size_t i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::move(value));
        _len++;
    }

    void insert(Copy, size_t index, T *first, size_t count) {
        if (_len + count > N) {
            panic("cap too large");
        }

        for (size_t i = _len; i > index; i--) {
            _buf[i] = _buf[i - count];
        }

        for (size_t i = 0; i < count; i++) {
            _buf[index + i] = first[i];
        }

        _len += count;
    }

    void insert(Move, size_t index, T *first, size_t count) {
        if (_len + count > N) {
            panic("cap too large");
        }

        for (size_t i = _len; i > index; i--) {
            _buf[i] = std::move<T>(_buf[i - count]);
        }

        for (size_t i = 0; i < count; i++) {
            _buf[index + i] = std::move(first[i]);
        }

        _len += count;
    }

    T removeAt(size_t index) {
        T tmp = _buf[index].take();
        for (size_t i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return tmp;
    }

    void resize(size_t newLen, T fill = {}) {
        if (newLen > _len) {
            ensure(newLen);
            for (size_t i = _len; i < newLen; i++) {
                _buf[i].ctor(fill);
            }
        } else if (newLen < _len) {
            for (size_t i = newLen; i < _len; i++) {
                _buf[i].dtor();
            }
        }
        _len = newLen;
    }

    void truncate(size_t newLen) {
        if (newLen >= _len)
            return;

        for (size_t i = newLen; i < _len; i++) {
            _buf[i].dtor();
        }

        _len = newLen;
    }

    T *buf() {
        return &_buf[0].unwrap();
    }

    T const *buf() const {
        return &_buf[0].unwrap();
    }

    size_t len() const {
        return _len;
    }

    size_t cap() const {
        return N;
    }
};

} // namespace Karm
