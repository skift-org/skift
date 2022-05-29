#pragma once

#include "keywords.h"
#include "ordr.h"
#include "rune.h"
#include "std.h"

namespace Karm {

template <Encoding E, typename U = typename E::Unit>
struct _Str : public Slice<U> {
    using Encoding = E;
    using Unit = U;

    using Slice<U>::Slice;

    constexpr _Str(Unit const *cstr) requires(Meta::Same<Unit, char>)
        : Slice<U>(cstr, strlen(cstr)) {}

    auto runes() const {
        Cursor<Unit> cursor(this->buf(), this->len());
        return Iter([cursor]() mutable -> Opt<Rune> {
            if (cursor.ended()) {
                return NONE;
            }

            Rune r;
            return E::decode(r, cursor) ? Opt<Rune>(r) : Opt<Rune>(NONE);
        });
    }
};

template <Encoding E, typename U = typename E::Unit>
struct _MutStr : public MutSlice<U> {
    using Encoding = E;
    using Unit = U;

    using MutSlice<U>::Slice;

    constexpr _MutStr(Unit *cstr) requires(Meta::Same<Unit, char>)
        : MutSlice<U>(cstr, strlen(cstr)) {}

    auto runes() const {
        Cursor<Unit> cursor(this->buf(), this->len());

        return Iter([cursor]() mutable -> Opt<Rune> {
            if (cursor.ended()) {
                return NONE;
            }

            Rune r;
            return E::decode(r, cursor) ? Opt<Rune>(r) : Opt<Rune>(NONE);
        });
    }
};

template <Encoding E>
struct _String {
    using Encoding = E;
    using Unit = typename E::Unit;

    Unit *_buf = nullptr;
    size_t _len = 0;

    _String() = default;

    _String(Adopt, Unit *buf, size_t len) : _buf(buf), _len(len) {}

    _String(Unit const *buf, size_t len) : _len(len) {
        _buf = new Unit[len + 1];
        _buf[len] = 0;
        memcpy(_buf, buf, len * sizeof(Unit));
    }

    _String(_Str<E> str) : _String(str.buf(), str.len()) {}

    _String(_String const &other) : _String(other._buf, other._len) {
    }

    _String(_String &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);
    }

    ~_String() {
        delete[] _buf;
    }

    operator _Str<E>() const {
        return {_buf, _len};
    }

    _String &operator=(_String const &other) {
        return *this = String(other);
    }

    _String &operator=(_String &&other) {
        std::swap(_buf, other._buf);
        std::swap(_len, other._len);

        return *this;
    }

    Unit const *begin() const { return _buf; }

    Unit const *end() const { return _buf + _len; }

    _Str<E> str() const { return {_buf, _len}; }

    auto units() const { return Slice<Unit>(_buf, _len).iter(); }

    auto runes() const {
        Cursor<Unit> cursor(_buf, _len);
        return Iter([cursor]() mutable -> Opt<Rune> {
            if (cursor.ended()) {
                return NONE;
            }

            Rune r;
            return E::decode(r, cursor) ? Opt<Rune>(r) : Opt<Rune>(NONE);
        });
    }

    auto cmp(_Str<E> const &other) const {
        return static_cast<_Str<E>>(*this).cmp(other);
    }

    Unit const *buf() const { return _buf; }

    size_t len() const { return _len; }

    template <::Encoding Target>
    _String<Target> transcoded() const {
        size_t len = transcode_len<Encoding, Target>(str());
        typename Target::Unit *buf = new typename Target::Unit[len + 1];

        Cursor<Unit> input = {_buf, _len};
        MutCursor<typename Target::Unit> output = {buf, len};

        transcode_units<Encoding, Target>(input, output);

        return {ADOPT, buf, len};
    }
};

using Str = _Str<Utf8>;

using MutStr = _MutStr<Utf8>;

using String = _String<Utf8>;

} // namespace Karm
