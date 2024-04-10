#pragma once

#include <karm-base/ctype.h>
#include <karm-base/cursor.h>
#include <karm-base/defer.h>
#include <karm-base/rune.h>
#include <karm-base/string.h>
#include <karm-meta/callable.h>

namespace Karm::Io {

template <StaticEncoding E>
struct _SScan {
    using Encoding = E;
    using Unit = typename E::Unit;

    Cursor<Unit> _cursor;
    Cursor<Unit> _begin;

    _SScan(_Str<E> str)
        : _cursor(str) {}

    /// Check if the scanner has reached the end of the input.
    /// This is equivalent to `rem() == 0`.
    bool ended() {
        return _cursor.ended();
    }

    /// Returns the number of runes remaining in the input.
    usize rem() {
        auto curr = _cursor;
        return transcodeLen<E>(curr);
    }

    /// Returns the remaining input as a string.
    _Str<E> remStr() {
        return {
            ::begin(_cursor),
            ::end(_cursor),
        };
    }

    /// Returns the current rune.
    Rune curr() {
        if (ended())
            return '\0';
        Rune r;
        auto curr = _cursor;
        return E::decodeUnit(r, curr) ? r : U'�';
    }

    /// Peek the next rune without advancing the cursor.
    Rune peek(usize count = 0) {
        auto save = _cursor;
        next(count);
        auto r = curr();
        _cursor = save;
        return r;
    }

    /// Return the current rune and advance the cursor.
    Rune next() {
        if (ended())
            return '\0';
        Rune r;
        return E::decodeUnit(r, _cursor) ? r : U'�';
    }

    /// Advance the cursor by `count` runes.
    Rune next(usize count) {
        Rune r = '\0';
        for (usize i = 0; i < count; i++)
            r = next();
        return r;
    }

    void begin() {
        _begin = _cursor;
    }

    _Str<E> end() {
        return {_begin, _cursor};
    }

    /// If the current rune is `c`, advance the cursor.
    bool skip(Rune c) {
        if (curr() == c) {
            next();
            return true;
        }
        return false;
    }

    /// If the current runes are `str`, advance the cursor.
    bool skip(Str str) {
        auto rollback = rollbackPoint();
        for (auto r : iterRunes(str))
            if (next() != r)
                return false;
        rollback.disarm();
        return true;
    }

    /// If the expression matches, advance the cursor.
    bool skip(auto expr)
        requires Meta::Callable<decltype(expr), decltype(*this)>
    {
        auto rollback = rollbackPoint();
        if (not expr(*this))
            return false;
        rollback.disarm();
        return true;
    }

    /// Keep advancing the cursor while the current rune is `c`.
    bool eat(Rune c) {
        bool result = false;
        if (skip(c)) {
            result = true;
            while (skip(c))
                ;
        }
        return result;
    }

    /// Keep advancing the cursor while the current runes are `str`.
    bool eat(Str str) {
        bool result = false;
        if (skip(str)) {
            result = true;
            while (skip(str))
                ;
        }
        return result;
    }

    /// Keep advancing the cursor while the expression matches.
    bool eat(auto expr) {
        bool result = false;
        if (skip(expr)) {
            result = true;
            while (skip(expr))
                ;
        }
        return result;
    }

    /// Check if the expression matches or not.
    /// The cursor is restored to the original position.
    Match match(auto expr) {
        auto rollback = rollbackPoint();
        if (expr(*this)) {
            Match result =
                ended()
                    ? Match::YES
                    : Match::PARTIAL;
            return result;
        }
        return Match::NO;
    }

    _Str<E> token(auto expr) {
        _begin = _cursor;
        if (not skip(expr))
            _cursor = _begin;
        return {_begin, _cursor};
    }

    /// Creates a rollback point for the scanner. If not manually disarmed,
    /// the scanner's state will be restored to its position at the time of
    /// this rollback point's creation when it goes out of scope.
    auto rollbackPoint() {
        return ArmedDefer{[&, saved = *this] {
            *this = saved;
        }};
    }
};

using SScan = _SScan<Utf8>;

} // namespace Karm::Io
