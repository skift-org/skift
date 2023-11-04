#pragma once

#include "rune.h"
#include "std.h"

namespace Karm {

inline constexpr usize strLen(char const *str) {
    usize len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

template <StaticEncoding E, typename U = typename E::Unit>
struct _Str : public Slice<U> {
    using Encoding = E;
    using Unit = U;

    constexpr _Str() = default;

    constexpr _Str(U const *cstr)
        requires(Meta::Same<U, char>)
        : Slice<U>(cstr, strLen(cstr)) {}

    constexpr _Str(U const *buf, usize len)
        : Slice<U>(buf, len) {}

    constexpr _Str(U const *begin, U const *end)
        : Slice<U>(begin, end - begin) {}

    constexpr _Str(Sliceable<U> auto const &other)
        : Slice<U>(other.buf(), other.len()) {}

    auto operator<=>(Unit const *cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return *this <=> _Str<E>(cstr);
    }

    auto operator==(Unit const *cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return *this == _Str<E>(cstr);
    }
};

template <StaticEncoding E>
struct _String {
    using Encoding = E;
    using Unit = typename E::Unit;
    using Inner = Unit;

    Unit *_buf;
    usize _len;

    _String()
        : _String("", 0) {}

    _String(Move, Unit *buf, usize len)
        : _buf(buf),
          _len(len) {
    }

    _String(Unit const *buf, usize len)
        : _len(len) {
        _buf = new Unit[len + 1];
        _buf[len] = 0;
        memcpy(_buf, buf, len * sizeof(Unit));
    }

    _String(Unit const *cstr)
        requires(Meta::Same<Unit, char>)
        : _String(cstr, strLen(cstr)) {}

    _String(_Str<E> str)
        : _String(str.buf(), str.len()) {}

    _String(_String const &other)
        : _String(other._buf, other._len) {
    }

    _String(_String &&other)
        : _buf(std::exchange(other._buf, nullptr)),
          _len(std::exchange(other._len, 0)) {
    }

    ~_String() {
        if (_buf) {
            delete[] _buf;
        }
    }

    _String &operator=(_String const &other) {
        *this = _String(other);
        return *this;
    }

    _String &operator=(_String &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);

        return *this;
    }

    _Str<E> str() const { return {_buf, _len}; }

    Slice<Unit> units() const {
        return {_buf, _len};
    }

    MutSlice<Unit> mutUnits() {
        return {_buf, _len};
    }

    Unit const &operator[](usize i) const { return _buf[i]; }
    Unit &operator[](usize i) { return _buf[i]; }
    Unit const *buf() const { return _buf; }
    Unit *buf() { return _buf; }
    usize len() const { return _len; }

    auto operator<=>(Unit const *cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return str() <=> _Str<E>(cstr);
    }

    auto operator==(Unit const *cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return str() == _Str<E>(cstr);
    }
};

template <StaticEncoding E>
struct _StringBuilder {
    Buf<typename E::Unit> _buf{};

    _StringBuilder(usize cap = 16)
        : _buf(cap) {}

    void append(Rune rune) {
        typename E::One one;
        if (not E::encodeUnit(rune, one)) {
            return;
        }

        for (auto unit : iter(one)) {
            _buf.insert(_buf.len(), std::move(unit));
        }
    }

    void append(_Str<E> str) {
        usize written = 0;
        for (auto rune : iterRunes(str)) {
            written += try$(append(rune));
        }
    }

    void append(Slice<typename E::Unit> unit) {
        _buf.insert(COPY, _buf.len(), unit.buf(), unit.len());
    }

    _Str<E> str() {
        return _buf;
    }

    void clear() {
        _buf.truncate(0);
    }

    _String<E> take() {
        usize len = _buf.size();
        _buf.insert(len, 0);
        return {MOVE, _buf.take(), len};
    }
};

using StringBuilder = _StringBuilder<Utf8>;

template <
    Sliceable S,
    typename E = typename S::Encoding,
    typename U = typename E::Unit>
auto iterRunes(S const &slice) {
    Cursor<U> cursor(slice);
    return Iter([cursor]() mutable -> Opt<Rune> {
        if (cursor.ended()) {
            return NONE;
        }

        Rune r;
        return E::decodeUnit(r, cursor) ? Opt<Rune>(r) : Opt<Rune>(NONE);
    });
}

template <StaticEncoding E>
bool eqCi(_Str<E> a, _Str<E> b) {
    if (a.len() != b.len())
        return false;

    Cursor<typename E::Unit> aCursor(a);
    Cursor<typename E::Unit> bCursor(b);

    while (not aCursor.ended()) {
        Rune aRune;
        Rune bRune;

        if (not E::decodeUnit(aRune, aCursor)) {
            return false;
        }

        if (not E::decodeUnit(bRune, bCursor)) {
            return false;
        }

        if (aRune != bRune and tolower(aRune) != tolower(bRune)) {
            return false;
        }
    }

    return true;
}

template <::StaticEncoding Target, ::StaticEncoding Source>
_String<Target> transcode(_Str<Source> str) {
    usize len = transcodeLen<Source, Target>(str);
    typename Target::Unit *buf = new typename Target::Unit[len + 1];
    buf[len] = '\0';

    Cursor<typename Source::Unit> input = str;
    MutSlice<typename Target::Unit> slice(buf, len);
    MutCursor<typename Target::Unit> output = slice;

    transcodeUnits<Source, Target>(input, output);

    return {MOVE, buf, len};
}

using Str = _Str<Utf8>;

using String = _String<Utf8>;

template <auto N>
struct StrLit {
    char _buf[N];
    constexpr StrLit(char const (&buf)[N]) {
        for (usize i = 0; i < N; i++) {
            _buf[i] = buf[i];
        }
    }
    constexpr operator Str() const { return _buf; }
    constexpr operator char const *() const { return _buf; }
};

} // namespace Karm
