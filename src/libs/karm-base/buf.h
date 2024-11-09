#pragma once

#include "array.h"
#include "clamp.h"
#include "manual.h"

namespace Karm {

#pragma clang unsafe_buffer_usage begin

/// A dynamically sized array of elements.
/// Often used as a backing store for other data structures. (e.g. `Vec`)
template <typename T>
struct Buf {
    using Inner = T;

    Manual<T> *_buf{};
    usize _cap{};
    usize _len{};

    static Buf init(usize len, T fill = {}) {
        Buf buf;
        buf.ensure(len);

        buf._len = len;
        for (usize i = 0; i < len; i++)
            buf._buf[i].ctor(fill);
        return buf;
    }

    Buf(usize cap = 0) {
        ensure(cap);
    }

    Buf(Move, T *buf, usize len)
        : _buf(buf),
          _cap(len),
          _len(len) {
    }

    Buf(std::initializer_list<T> other) {
        ensure(other.size());

        _len = other.size();
        for (usize i = 0; i < _len; i++)
            _buf[i].ctor(std::move(other.begin()[i]));
    }

    Buf(Sliceable<T> auto const &other) {
        ensure(other.len());

        _len = other.len();
        for (usize i = 0; i < _len; i++)
            _buf[i].ctor(other[i]);
    }

    Buf(Buf const &other) {
        ensure(other._len);

        _len = other._len;
        for (usize i = 0; i < _len; i++)
            _buf[i].ctor(other[i]);
    }

    Buf(Buf &&other) {
        std::swap(_buf, other._buf);
        std::swap(_cap, other._cap);
        std::swap(_len, other._len);
    }

    ~Buf() {
        if (not _buf)
            return;
        for (usize i = 0; i < _len; i++)
            _buf[i].dtor();
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

    constexpr T &operator[](usize i) lifetimebound {
        return _buf[i].unwrap();
    }

    constexpr T const &operator[](usize i) const lifetimebound {
        return _buf[i].unwrap();
    }

    void ensure(usize desired) {
        if (desired <= _cap)
            return;

        if (not _buf) {
            _buf = new Manual<T>[desired];
            _cap = desired;
            return;
        }

        usize newCap = max(_cap * 2, desired);

        Manual<T> *tmp = new Manual<T>[newCap];
        for (usize i = 0; i < _len; i++) {
            tmp[i].ctor(_buf[i].take());
        }

        delete[] _buf;
        _buf = tmp;
        _cap = newCap;
    }

    void fit() {
        if (_len == _cap)
            return;

        Manual<T> *tmp = nullptr;

        if (_len) {
            tmp = new Manual<T>[_len];
            for (usize i = 0; i < _len; i++)
                tmp[i].ctor(_buf[i].take());
        }

        delete[] _buf;
        _buf = tmp;
        _cap = _len;
    }

    template <typename... Args>
    auto &emplace(usize index, Args &&...args) {
        ensure(_len + 1);

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::forward<Args>(args)...);
        _len++;
        return _buf[index].unwrap();
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
        if (index >= _len) [[unlikely]]
            panic("index out of bounds");

        T ret = _buf[index].take();
        for (usize i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return ret;
    }

    void removeRange(usize index, usize count) {
        if (index > _len) [[unlikely]]
            panic("index out of bounds");

        if (index + count > _len) [[unlikely]]
            panic("index + count out of bounds");

        for (usize i = index; i < _len - count; i++)
            _buf[i].ctor(_buf[i + count].take());

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

    void trunc(usize newLen) {
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

    T *buf() lifetimebound {
        if (_buf == nullptr)
            return nullptr;
        return &_buf->unwrap();
    }

    T const *buf() const lifetimebound {
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

    Array<Manual<T>, N> _buf = {};
    usize _len = {};

    constexpr InlineBuf() = default;

    InlineBuf(usize cap) {
        if (cap > N) [[unlikely]]
            panic("cap too large");
    }

    InlineBuf(T const *buf, usize len) {
        if (len > N) [[unlikely]]
            panic("len too large");

        _len = len;
        for (usize i = 0; i < _len; i++)
            _buf[i].ctor(buf[i]);
    }

    InlineBuf(std::initializer_list<T> other)
        : InlineBuf(other.begin(), other.size()) {
    }

    InlineBuf(Sliceable<T> auto const &other)
        : InlineBuf(other.buf(), other.len()) {
    }

    InlineBuf(InlineBuf const &other)
        : InlineBuf(other.buf(), other.len()) {
    }

    InlineBuf(InlineBuf &&other) {
        for (usize i = 0; i < N; i++) {
            _buf[i].ctor(std::move(other._buf[i].take()));
        }
        _len = other._len;
    }

    ~InlineBuf() {
        for (usize i = 0; i < _len; i++) {
            _buf[i].dtor();
        }
        _len = 0;
    }

    InlineBuf &operator=(InlineBuf const &other) {
        *this = InlineBuf(other);
        return *this;
    }

    InlineBuf &operator=(InlineBuf &&other) {
        for (usize i = 0; i < min(_len, other._len); i++)
            buf()[i] = std::move(other.buf()[i]);

        for (usize i = _len; i < other._len; i++)
            _buf[i].ctor(std::move(other._buf[i].take()));

        for (usize i = other._len; i < _len; i++)
            _buf[i].dtor();

        _len = other._len;

        return *this;
    }

    constexpr T &operator[](usize i) lifetimebound {
        return _buf[i].unwrap();
    }

    constexpr T const &operator[](usize i) const lifetimebound {
        return _buf[i].unwrap();
    }

    void ensure(usize len) {
        if (len > N) [[unlikely]]
            panic("cap too large");
    }

    void fit() {
        // no-op
    }

    template <typename... Args>
    void emplace(usize index, Args &&...args) {
        if (_len == N) [[unlikely]]
            panic("cap too large");

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::forward<Args>(args)...);
        _len++;
    }

    void insert(usize index, T &&value) {
        if (_len == N) [[unlikely]]
            panic("cap too large");

        for (usize i = _len; i > index; i--) {
            _buf[i].ctor(_buf[i - 1].take());
        }

        _buf[index].ctor(std::move(value));
        _len++;
    }

    void insert(Copy, usize index, T *first, usize count) {
        if (_len + count > N) [[unlikely]]
            panic("cap too large");

        for (usize i = _len; i > index; i--) {
            _buf[i] = _buf[i - count];
        }

        for (usize i = 0; i < count; i++) {
            _buf[index + i] = first[i];
        }

        _len += count;
    }

    void insert(Move, usize index, T *first, usize count) {
        if (_len + count > N) [[unlikely]]
            panic("cap too large");

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

    void trunc(usize newLen) {
        if (newLen >= _len)
            return;

        for (usize i = newLen; i < _len; i++) {
            _buf[i].dtor();
        }

        _len = newLen;
    }

    T *buf() lifetimebound {
        return &_buf[0].unwrap();
    }

    T const *buf() const lifetimebound {
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

    Manual<T> *_buf{};
    usize _cap{};
    usize _len{};

    ViewBuf() = default;

    ViewBuf(Manual<T> *buf, usize cap)
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
        if (cap > _cap) [[unlikely]]
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
        if (index >= _len) [[unlikely]]
            panic("index out of bounds");

        T ret = _buf[index].take();
        for (usize i = index; i < _len - 1; i++) {
            _buf[i].ctor(_buf[i + 1].take());
        }
        _len--;
        return ret;
    }

    void removeRange(usize index, usize count) {
        if (index > _len) [[unlikely]]
            panic("index out of bounds");

        if (index + count > _len) [[unlikely]]
            panic("index + count out of bounds");

        for (usize i = index; i < _len - count; i++)
            _buf[i].ctor(_buf[i + count].take());

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

    void trunc(usize newLen) {
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

#pragma clang unsafe_buffer_usage end

} // namespace Karm
