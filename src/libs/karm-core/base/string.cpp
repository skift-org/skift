module;

#include <karm-core/macros.h>

export module Karm.Core:base.string;

import :base.cstr;
import :base.ctype;
import :base.rune;
import :base.slice;

namespace Karm {

export template <StaticEncoding E, typename U = typename E::Unit>
struct _Str : Slice<U> {
    using Encoding = E;
    using Unit = U;

    always_inline constexpr _Str() = default;

    always_inline constexpr _Str(U const* cstr)
        requires(Meta::Same<U, char>)
        : Slice<U>(cstr, cstrLen(cstr)) {}

    always_inline constexpr _Str(U const* buf, usize len)
        : Slice<U>(buf, len) {}

    always_inline constexpr _Str(U const* begin, U const* end)
        : Slice<U>(begin, end - begin) {}

    always_inline constexpr _Str(Sliceable<U> auto const& other)
        : Slice<U>(other.buf(), other.len()) {}

    always_inline constexpr auto operator<=>(Unit const* cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return *this <=> _Str<E>(cstr);
    }

    always_inline constexpr bool operator==(Unit const* cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return *this == _Str<E>(cstr);
    }

    always_inline constexpr explicit operator bool() const {
        return this->_len > 0;
    }
};

export template <StaticEncoding E, typename U>
struct Niche<_Str<E, U>> {
    struct Content : public Niche<Slice<U>>::Content {};
};

export template <StaticEncoding E, usize N>
struct _InlineString {
    using Encoding = E;
    using Unit = typename E::Unit;
    using Inner = Unit;

    Array<Unit, N> _buf;
    u8 _len = 0;
    static_assert(N <= Limits<u8>::MAX, "N too large");

    constexpr _InlineString() = default;

    _InlineString(Unit const* buf, usize len) {
        if (len > N) [[unlikely]]
            panic("len too large");

        _len = min(len, N);
        for (usize i = 0; i < _len; i++)
            _buf[i] = buf[i];
    }

    always_inline _InlineString(_Str<E> str)
        : _InlineString(str.buf(), str.len()) {}

    always_inline _InlineString(Sliceable<Unit> auto const& other)
        : _InlineString(other.buf(), other.len()) {}

    always_inline _Str<E> str() const lifetimebound { return *this; }

    always_inline Unit const& operator[](usize i) const lifetimebound {
        return _buf[i];
    }

    always_inline Unit const* buf() const lifetimebound { return _buf.buf(); }

    always_inline usize len() const { return _len; }

    always_inline auto operator<=>(Unit const* cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return str() <=> _Str<E>(cstr);
    }

    always_inline bool operator==(Unit const* cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return str() == _Str<E>(cstr);
    }

    always_inline constexpr explicit operator bool() const {
        return _len > 0;
    }
};

export template <StaticEncoding E, usize N>
    requires Nicheable<Array<typename E::Unit, N>>
struct Niche<_InlineString<E, N>> {
    struct Content : public Niche<Array<typename E::Unit, N>>::Content {};
};

export template <usize N>
using InlineString = _InlineString<Utf8, N>;

export template <StaticEncoding E>
struct _String {
    using Encoding = E;
    using Unit = typename E::Unit;
    using Inner = Unit;

    static constexpr Array<Unit, 1> _EMPTY = {0};

    Unit* _buf = nullptr;
    usize _len = 0;

    constexpr _String() = default;

    always_inline _String(Move, Unit* buf, usize len)
        : _buf(buf),
          _len(len) {
    }

    _String(Unit const* buf, usize len)
        : _len(len) {
        if (len == 0)
            // Allow initializing the string using "" and not allocating memory.
            return;

        auto store = new Unit[len + 1];
        store[len] = 0;
        std::memcpy(store, buf, len * sizeof(Unit));
        _buf = store;
    }

    always_inline _String(_Str<E> str)
        : _String(str.buf(), str.len()) {}

    always_inline _String(Sliceable<Unit> auto const& other)
        : _String(other.buf(), other.len()) {}

    always_inline _String(_String const& other)
        : _String(other._buf, other._len) {
    }

    always_inline _String(_String&& other)
        : _buf(std::exchange(other._buf, nullptr)),
          _len(std::exchange(other._len, 0)) {
    }

    ~_String() {
        if (_buf) {
            _len = 0;
            delete[] std::exchange(_buf, nullptr);
        }
    }

    always_inline _String& operator=(_String const& other) {
        *this = _String(other);
        return *this;
    }

    always_inline _String& operator=(_String&& other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
        return *this;
    }

    always_inline _Str<E> str() const lifetimebound { return *this; }

    always_inline Unit const& operator[](usize i) const lifetimebound {
        if (i >= _len) [[unlikely]]
            panic("index out of bounds");
        return buf()[i];
    }

    always_inline Unit const* buf() const lifetimebound { return _len ? _buf : _EMPTY.buf(); }

    always_inline usize len() const { return _len; }

    always_inline auto operator<=>(Unit const* cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return str() <=> _Str<E>(cstr);
    }

    always_inline bool operator==(Unit const* cstr) const
        requires(Meta::Same<Unit, char>)
    {
        return str() == _Str<E>(cstr);
    }

    always_inline constexpr explicit operator bool() const {
        return _len > 0;
    }
};

export template <StaticEncoding E>
struct Niche<_String<E>> {
    struct Content {
        char const* ptr;
        usize _len;

        always_inline constexpr Content() : ptr(NICHE_PTR) {}

        always_inline constexpr bool has() const {
            return ptr != NICHE_PTR;
        }
    };
};

export template <
    Sliceable S,
    typename E = typename S::Encoding,
    typename U = typename E::Unit>
auto iterRunes(S const& slice) {
    Cursor<U> cursor(slice);
    return Iter([cursor] mutable -> Opt<Rune> {
        if (cursor.ended()) {
            return NONE;
        }

        Rune r;
        return E::decodeUnit(r, cursor) ? Opt<Rune>(r) : Opt<Rune>(NONE);
    });
}

export template <StaticEncoding E>
bool eqCi(_Str<E> a, _Str<E> b) {
    if (a.len() != b.len())
        return false;

    Cursor<typename E::Unit> aCursor{a};
    Cursor<typename E::Unit> bCursor{b};

    while (not aCursor.ended()) {
        Rune aRune;
        Rune bRune;

        if (not E::decodeUnit(aRune, aCursor))
            return false;

        if (not E::decodeUnit(bRune, bCursor))
            return false;

        if (aRune != bRune and toAsciiLower(aRune) != toAsciiLower(bRune))
            return false;
    }

    return true;
}

export template <StaticEncoding Target, StaticEncoding Source>
_String<Target> transcode(_Str<Source> str) {
    usize len = transcodeLen<Source, Target>(str);
    typename Target::Unit* buf = new typename Target::Unit[len + 1];
    buf[len] = '\0';

    Cursor<typename Source::Unit> input = str;
    MutSlice<typename Target::Unit> slice(buf, len);
    MutCursor<typename Target::Unit> output = slice;

    transcodeUnits<Source, Target>(input, output);

    return {MOVE, buf, len};
}

export using Str = _Str<Utf8>;

export using String = _String<Utf8>;

export template <auto N>
struct StrLit {
    char _buf[N];

    constexpr StrLit(char const (&buf)[N]) {
        for (usize i = 0; i < N; i++) {
            _buf[i] = buf[i];
        }
    }

    constexpr operator Str() const lifetimebound { return _buf; }

    constexpr operator char const*() const lifetimebound { return _buf; }
};

// MARK: String Conversion -----------------------------------------------------

export template <StaticEncoding E>
struct _StringBuilder {
    Buf<typename E::Unit> _buf{};

    _StringBuilder(usize cap = 16)
        : _buf(cap) {}

    _StringBuilder(String&& str)
        : _buf(MOVE, std::exchange(str._buf, nullptr), std::exchange(str._len, 0)) {
    }

    void ensure(usize cap) {
        // NOTE: This way client code don't have to take
        //       the null-terminator into account
        _buf.ensure(cap + 1);
    }

    void append(Rune rune) {
        typename E::One one;
        if (not E::encodeUnit(rune, one))
            return;

        for (auto unit : iter(one))
            _buf.insert(_buf.len(), std::move(unit));
    }

    void append(Sliceable<Rune> auto const& runes) {
        for (auto rune : runes)
            append(rune);
    }

    void append(Sliceable<typename E::Unit> auto const& units) {
        _buf.insert(COPY, _buf.len(), units.buf(), units.len());
    }

    usize len() const {
        return _buf.size();
    }

    _Str<E> str() const lifetimebound {
        return _buf;
    }

    Bytes bytes() const lifetimebound {
        return Karm::bytes(_buf);
    }

    void clear() {
        _buf.trunc(0);
    }

    _String<E> take() {
        usize len = _buf.len();
        _buf.insert(len, 0);
        return {MOVE, _buf.take(), len};
    }
};

export using StringBuilder = _StringBuilder<Utf8>;

} // namespace Karm

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuser-defined-literals"

export constexpr Karm::Str operator""s(char const* buf, Karm::usize len) {
    return {buf, len};
}

export constexpr Karm::_Str<Karm::Utf8> operator""_s8(char const* buf, Karm::usize len) {
    return {buf, len};
}

export constexpr Karm::_Str<Karm::Utf16> operator""_s16(char16_t const* buf, Karm::usize len) {
    return {reinterpret_cast<Karm::u16 const*>(buf), len};
}

#pragma clang diagnostic pop
