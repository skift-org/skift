#pragma once

#include "keywords.h"
#include "ordr.h"
#include "rune.h"
#include "std.h"

namespace Karm {

static constexpr size_t strLen(char const *str) {
    size_t len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

template <StaticEncoding E, typename U = typename E::Unit>
struct _Str : public Slice<U> {
    using Encoding = E;
    using Unit = U;

    using Slice<U>::Slice;

    constexpr _Str(Unit const *cstr) requires(Meta::Same<Unit, char>)
        : Slice<U>(cstr, strLen(cstr)) {}
};

template <StaticEncoding E, typename U = typename E::Unit>
struct _MutStr : public MutSlice<U> {
    using Encoding = E;
    using Unit = U;

    using MutSlice<U>::MutSlice;

    constexpr _MutStr(Unit *cstr) requires(Meta::Same<Unit, char>)
        : MutSlice<U>(cstr, strLen(cstr)) {}
};

template <StaticEncoding E>
struct _String {
    using Encoding = E;
    using Unit = typename E::Unit;
    using Inner = Unit;

    Unit *_buf = nullptr;
    size_t _len = 0;

    _String() = default;

    _String(Move, Unit *buf, size_t len)
        : _buf(buf), _len(len) {
    }

    _String(Unit const *buf, size_t len)
        : _len(len) {
        _buf = new Unit[len + 1];
        _buf[len] = 0;
        memcpy(_buf, buf, len * sizeof(Unit));
    }

    _String(_Str<E> str)
        : _String(str.buf(), str.len()) {}

    _String(_String const &other)
        : _String(other._buf, other._len) {
    }

    _String(_String &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
    }

    ~_String() {
        if (_buf) {
            delete[] _buf;
        }
    }

    _String &operator=(_String const &other) {
        return *this = _String(other);
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

    Unit const &operator[](size_t i) const { return _buf[i]; }
    Unit &operator[](size_t i) { return _buf[i]; }
    Unit const *buf() const { return _buf; }
    Unit *buf() { return _buf; }
    size_t len() const { return _len; }
};

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

template <::StaticEncoding Target, ::StaticEncoding Source>
_String<Target> transcode(_Str<Source> str) {
    size_t len = transcodeLen<Source, Target>(str);
    typename Target::Unit *buf = new typename Target::Unit[len + 1];
    buf[len] = '\0';

    Cursor<typename Source::Unit> input = str;
    MutSlice<typename Target::Unit> slice(buf, len);
    MutCursor<typename Target::Unit> output = slice;

    transcodeUnits<Source, Target>(input, output);

    return {MOVE, buf, len};
}

using Str = _Str<Utf8>;

using MutStr = _MutStr<Utf8>;

using String = _String<Utf8>;

} // namespace Karm
