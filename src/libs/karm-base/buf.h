#pragma once

#include "array.h"
#include "clamp.h"
#include "inert.h"

namespace Karm {

/// A dynamically sized array of elements.
/// Often used as a backing store for other data structures. (e.g. `Vec`)
template <typename T>
struct Buf {
    using Inner = T;

    Inert<T> *_buf{};
    usize _cap{};
    usize _len{};

    static Buf init(usize len, T fill = {}) {
        Buf buf;
        buf._cap = len;
        buf._len = len;
        buf._buf = new Inert<T>[len];
        for (usize i = 0; i < len; i++) {
            buf._buf[i].ctor(fill);
        }
        return buf;
    }

    Buf() = default;

    Buf(usize cap)
        : _cap(cap) {
        _buf = new Inert<T>[cap];
    }

    Buf(std::initializer_list<T> other) {
        _cap = other.size();
        _len = other.size();
        _buf = new Inert<T>[_cap];
        for (usize i = 0; i < _len; i++) {
            _buf[i].ctor(std::move(other.begin()[i]));
        }
    }

    Buf(Sliceable<T> auto &other) {
        _cap = other.len();
        _len = other.len();
        _buf = new Inert<T>[_cap];
        for (usize i = 0; i < _len; i++) {
            _buf[i].ctor(other[i]);
        }
    }

    Buf(Buf const &other) {
        _cap = other._cap;
        _len = other._len;
        _buf = new Inert<T>[_cap];

        for (usize i = 0; i < _len; i++) {
            _buf[i].ctor(other[i]);
        }
    }

    Buf(Buf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
    }

    ~Buf() {
        if (not _buf)
            return;

        for (usize i = 0; i < _len; i++) {
            _buf[i].dtor();
        }

        delete[] _buf;
    }

    Buf &operator=(Buf const &other) {
        *this = Buf(other);
        return *this;
    }

    Buf &operator=(Buf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
        return *this;
    }

    constexpr T &operator[](usize i) { return _buf[i].unwrap(); }

    constexpr T const &operator[](usize i) const { return _buf[i].unwrap(); }

    void ensure(usize cap) {
        if (cap <= _cap)
            return;

        if (not _buf) {
            _buf = new Inert<T>[cap];
            _cap = cap;
            return;
        }

        Inert<T> *tmp = new Inert<T>[cap];
        for (usize i = 0; i < _len; i++) {
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
        for (usize i = 0; i < _len; i++) {
            tmp[i].ctor(_buf[i].take());
        }

        delete[] _buf;
        _buf = tmp;
        _cap = _len;
    }

    template <typename... Args>
    void emplace(usize index, Args &&...args) {
        ensure(_len + 1);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }
        _buf[index].ctor(std::forward<Args>(args)...);
        _len++;
    }

    void insert(usize index, T &&value) {
        ensure(_len + 1);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::move(value));
        _len++;
    }

    void replace(usize index, T &&value) {
        if (index >= _len) {
            insert(index, std::move(value));
            return;
        }

        _buf[index].dtor();
        _buf[index].ctor(std::move(value));
    }

    void insert(Copy, usize index, T const *first, usize count) {
        ensure(_len + count);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - count].take());
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i].ctor(first[i]);
        }

        _len += count;
    }

    void insert(Move, usize index, T *first, usize count) {
        ensure(_len + count);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - count].take());
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i].ctor(std::move(first[i]));
        }

        _len += count;
    }

    T removeAt(usize index) {
        if (index >= _len) {
            panic("index out of bounds");
        }

        T ret = _buf[index].take();
        for (usize i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return ret;
    }

    void removeRange(usize index, usize count) {
        if (index > _len) {
            panic("index out of bounds");
        }

        if (index + count > _len) {
            panic("index + count out of bounds");
        }

        for (usize i = index; i < _len - count; i++) {
            _buf[i].ctor(_buf[i + count].take());
        }

        _len -= count;
    }

    void resize(usize newLen, T fill = {}) {
        if (newLen > _len) {
            ensure(newLen);
            for (usize i = _len; i < newLen; i++) {
                _buf[i].ctor(fill);
            }
        } else if (newLen < _len) {
            for (usize i = newLen; i < _len; i++) {
                _buf[i].dtor();
            }
        }
        _len = newLen;
    }

    void truncate(usize newLen) {
        if (newLen >= _len)
            return;

        for (usize i = newLen; i < _len; i++) {
            _buf[i].dtor();
        }

        _len = newLen;
    }

    T *take() {
        T *ret = buf();
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

    usize len() const {
        return _len;
    }

    usize cap() const {
        return _cap;
    }

    usize size() const {
        return _len * sizeof(T);
    }

    void leak() {
        _buf = nullptr;
        _cap = 0;
        _len = 0;
    }
};

/// A buffer that uses inline storage, great for small buffers.
template <typename T, usize N>
struct InlineBuf {
    using Inner = T;

    Array<Inert<T>, N> _buf = {};
    usize _len = {};

    constexpr InlineBuf() = default;

    InlineBuf(usize cap) {
        if (cap > N) {
            panic("cap too large");
        }
    }

    InlineBuf(std::initializer_list<T> other) {
        _len = other.size();
        if (_len > N) {
            panic("cap too large");
        }
        for (usize i = 0; i < _len; i++) {
            _buf[i].ctor(std::move(other.begin()[i]));
        }
    }

    InlineBuf(Sliceable<T> auto &other) {
        _len = other.len();

        if (_len > N) {
            panic("cap too large");
        }

        for (usize i = 0; i < _len; i++) {
            _buf[i].ctor(other[i]);
        }
    }

    InlineBuf(InlineBuf const &other) {
        _len = other._len;
        if (_len > N) {
            panic("cap too large");
        }
        for (usize i = 0; i < _len; i++) {
            _buf[i].ctor(other[i]);
        }
    }

    InlineBuf(InlineBuf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
    }

    ~InlineBuf() = default;

    InlineBuf &operator=(InlineBuf const &other) {
        *this = InlineBuf(other);
        return *this;
    }

    InlineBuf &operator=(InlineBuf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
        return *this;
    }

    constexpr T &operator[](usize i) { return _buf[i].unwrap(); }

    constexpr T const &operator[](usize i) const { return _buf[i].unwrap(); }

    void ensure(usize len) {
        if (len > N) {
            panic("cap too large");
        }
    }

    void fit() {
        // no-op
    }

    template <typename... Args>
    void emplace(usize index, Args &&...args) {
        if (_len == N) {
            panic("cap too large");
        }

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::forward<Args>(args)...);
        _len++;
    }

    void insert(usize index, T &&value) {
        if (_len == N) {
            panic("cap too large");
        }

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::move(value));
        _len++;
    }

    void insert(Copy, usize index, T *first, usize count) {
        if (_len + count > N) {
            panic("cap too large");
        }

        for (usize i = _len; i > index; i--) {
            _buf[i] = _buf[i - count];
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i] = first[i];
        }

        _len += count;
    }

    void insert(Move, usize index, T *first, usize count) {
        if (_len + count > N) {
            panic("cap too large");
        }

        for (usize i = _len; i > index; i--) {
            _buf[i] = std::move<T>(_buf[i - count]);
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i] = std::move(first[i]);
        }

        _len += count;
    }

    T removeAt(usize index) {
        T tmp = _buf[index].take();
        for (usize i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return tmp;
    }

    void resize(usize newLen, T fill = {}) {
        if (newLen > _len) {
            ensure(newLen);
            for (usize i = _len; i < newLen; i++) {
                _buf[i].ctor(fill);
            }
        } else if (newLen < _len) {
            for (usize i = newLen; i < _len; i++) {
                _buf[i].dtor();
            }
        }
        _len = newLen;
    }

    void truncate(usize newLen) {
        if (newLen >= _len)
            return;

        for (usize i = newLen; i < _len; i++) {
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

    usize len() const {
        return _len;
    }

    usize cap() const {
        return N;
    }
};

/// A buffer that does not own its backing storage.
template <typename T>
struct ViewBuf {
    using Inner = T;

    Inert<T> *_buf{};
    usize _cap{};
    usize _len{};

    ViewBuf() = default;

    ViewBuf(Inert<T> *buf, usize cap)
        : _buf(buf), _cap(cap) {
    }

    ViewBuf(ViewBuf const &other) {
        _cap = other._cap;
        _len = other._len;
        _buf = other._buf;
    }

    ViewBuf(ViewBuf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
    }

    ~ViewBuf() {}

    ViewBuf &operator=(ViewBuf const &other) {
        *this = ViewBuf(other);
        return *this;
    }

    ViewBuf &operator=(ViewBuf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
        return *this;
    }

    constexpr T &operator[](usize i) {
        return _buf[i].unwrap();
    }

    constexpr T const &operator[](usize i) const {
        return _buf[i].unwrap();
    }

    void ensure(usize cap) {
        if (cap <= _cap)
            return;

        panic("cap too large");
    }

    void fit() {
    }

    template <typename... Args>
    void emplace(usize index, Args &&...args) {
        ensure(_len + 1);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }
        _buf[index].ctor(std::forward<Args>(args)...);
        _len++;
    }

    void insert(usize index, T &&value) {
        ensure(_len + 1);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::move(value));
        _len++;
    }

    void replace(usize index, T &&value) {
        if (index >= _len) {
            insert(index, std::move(value));
            return;
        }

        _buf[index].dtor();
        _buf[index].ctor(std::move(value));
    }

    void insert(Copy, usize index, T *first, usize count) {
        ensure(_len + count);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - count].take());
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i].ctor(first[i]);
        }

        _len += count;
    }

    void insert(Move, usize index, T *first, usize count) {
        ensure(_len + count);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - count].take());
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i].ctor(std::move(first[i]));
        }

        _len += count;
    }

    T removeAt(usize index) {
        if (index >= _len) {
            panic("index out of bounds");
        }

        T ret = _buf[index].take();
        for (usize i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return ret;
    }

    void removeRange(usize index, usize count) {
        if (index > _len) {
            panic("index out of bounds");
        }

        if (index + count > _len) {
            panic("index + count out of bounds");
        }

        for (usize i = index; i < _len - count; i++) {
            _buf[i].ctor(_buf[i + count].take());
        }

        _len -= count;
    }

    void resize(usize newLen, T fill = {}) {
        if (newLen > _len) {
            ensure(newLen);
            for (usize i = _len; i < newLen; i++) {
                _buf[i].ctor(fill);
            }
        } else if (newLen < _len) {
            for (usize i = newLen; i < _len; i++) {
                _buf[i].dtor();
            }
        }
        _len = newLen;
    }

    void truncate(usize newLen) {
        if (newLen >= _len)
            return;

        for (usize i = newLen; i < _len; i++) {
            _buf[i].dtor();
        }

        _len = newLen;
    }

    T *take() {
        T *ret = buf();
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

    usize len() const {
        return _len;
    }

    usize cap() const {
        return _cap;
    }

    usize size() const {
        return _len * sizeof(T);
    }
};

} // namespace Karm
