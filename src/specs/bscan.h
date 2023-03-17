#pragma once

#include <karm-base/cursor.h>
#include <karm-base/endiant.h>
#include <karm-base/string.h>

struct BScan {
    Cursor<u8> _cursor;

    ALWAYS_INLINE constexpr BScan(Bytes bytes) : _cursor(bytes) {}

    ALWAYS_INLINE constexpr bool ended() {
        return _cursor.ended();
    }

    ALWAYS_INLINE constexpr usize rem() {
        return _cursor.rem();
    }

    ALWAYS_INLINE constexpr BScan &skip(usize n) {
        n = min(n, rem());
        _cursor.next(n);
        return *this;
    }

    ALWAYS_INLINE constexpr BScan peek(usize n) {
        BScan c{*this};
        c.skip(n);
        return c;
    }

    template <typename T>
    ALWAYS_INLINE constexpr bool readTo(T *buf, usize n) {
        if (rem() < n) {
            return false;
        }

        u8 *b = reinterpret_cast<u8 *>(buf);
        for (usize i = 0; i < n; i++) {
            b[i] = _cursor.next();
        }
        return true;
    }

    template <typename T>
    ALWAYS_INLINE constexpr bool peekTo(T *buf, usize n) {
        if (rem() < n) {
            return false;
        }

        u8 *b = reinterpret_cast<u8 *>(buf);
        for (usize i = 0; i < n; i++) {
            b[i] = _cursor.buf()[i];
        }
        return true;
    }

    template <typename T>
    ALWAYS_INLINE constexpr T nextBe() {
        _be<T> r{};
        readTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    ALWAYS_INLINE constexpr T nextLe() {
        _le<T> r;
        readTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    ALWAYS_INLINE constexpr T peekBe() {
        _be<T> r{};
        peekTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    ALWAYS_INLINE constexpr T peekLe() {
        _be<T> r;
        peekTo(&r, sizeof(T));
        return r;
    }

    ALWAYS_INLINE constexpr u8 nextU8be() { return nextBe<u8>(); }
    ALWAYS_INLINE constexpr u16 nextU16be() { return nextBe<u16>(); }
    ALWAYS_INLINE constexpr u32 nextU32be() { return nextBe<u32>(); }
    ALWAYS_INLINE constexpr u64 nextU64be() { return nextBe<u64>(); }

    ALWAYS_INLINE constexpr u8 nextU8le() { return nextLe<u8>(); }
    ALWAYS_INLINE constexpr u16 nextU16le() { return nextLe<u16>(); }
    ALWAYS_INLINE constexpr u32 nextU32le() { return nextLe<u32>(); }
    ALWAYS_INLINE constexpr u64 nextU64le() { return nextLe<u64>(); }

    ALWAYS_INLINE constexpr i8 nextI8be() { return nextBe<i8>(); }
    ALWAYS_INLINE constexpr i16 nextI16be() { return nextBe<i16>(); }
    ALWAYS_INLINE constexpr i32 nextI32be() { return nextBe<i32>(); }
    ALWAYS_INLINE constexpr i64 nextI64be() { return nextBe<i64>(); }

    ALWAYS_INLINE constexpr i8 nextI8le() { return nextLe<i8>(); }
    ALWAYS_INLINE constexpr i16 nextI16le() { return nextLe<i16>(); }
    ALWAYS_INLINE constexpr i32 nextI32le() { return nextLe<i32>(); }
    ALWAYS_INLINE constexpr i64 nextI64le() { return nextLe<i64>(); }

    ALWAYS_INLINE constexpr u8 peekU8be() { return peekBe<u8>(); }
    ALWAYS_INLINE constexpr u16 peekU16be() { return peekBe<u16>(); }
    ALWAYS_INLINE constexpr u32 peekU32be() { return peekBe<u32>(); }
    ALWAYS_INLINE constexpr u64 peekU64be() { return peekBe<u64>(); }

    ALWAYS_INLINE constexpr u8 peekU8le() { return peekLe<u8>(); }
    ALWAYS_INLINE constexpr u16 peekU16le() { return peekLe<u16>(); }
    ALWAYS_INLINE constexpr u32 peekU32le() { return peekLe<u32>(); }
    ALWAYS_INLINE constexpr u64 peekU64le() { return peekLe<u64>(); }

    ALWAYS_INLINE constexpr i8 peekI8be() { return peekBe<i8>(); }
    ALWAYS_INLINE constexpr i16 peekI16be() { return peekBe<i16>(); }
    ALWAYS_INLINE constexpr i32 peekI32be() { return peekBe<i32>(); }
    ALWAYS_INLINE constexpr i64 peekI64be() { return peekBe<i64>(); }

    ALWAYS_INLINE constexpr i8 peekI8le() { return peekLe<i8>(); }
    ALWAYS_INLINE constexpr i16 peekI16le() { return peekLe<i16>(); }
    ALWAYS_INLINE constexpr i32 peekI32le() { return peekLe<i32>(); }
    ALWAYS_INLINE constexpr i64 peekI64le() { return peekLe<i64>(); }

    ALWAYS_INLINE constexpr Str nextStr(usize n) {
        n = clamp(n, 0uz, rem());
        Str s{(char const *)_cursor.buf(), n};
        _cursor.next(n);
        return s;
    }

    ALWAYS_INLINE constexpr Str nextCStr() {
        usize n = 0;
        while (n < rem() and _cursor.buf()[n] != '\0') {
            n++;
        }
        return nextStr(n);
    }

    ALWAYS_INLINE constexpr Bytes nextBytes(usize n) {
        n = clamp(n, 0uz, rem());
        Bytes b{_cursor.buf(), n};
        _cursor.next(n);
        return b;
    }

    ALWAYS_INLINE constexpr Bytes restBytes() {
        return nextBytes(rem());
    }
};

template <typename T, usize Offset>
struct BField {
    using Type = T;
    static constexpr usize offset = Offset;
};

struct BChunk {
    Bytes _slice;

    ALWAYS_INLINE constexpr BChunk() = default;

    ALWAYS_INLINE constexpr BChunk(Bytes slice) : _slice(slice) {}

    ALWAYS_INLINE constexpr bool present() const {
        return _slice.len() > 0;
    }

    ALWAYS_INLINE constexpr BScan begin() const {
        return _slice;
    }

    template <typename T>
    ALWAYS_INLINE constexpr typename T::Type get() const {
        typename T::Type r{};
        begin()
            .skip(T::offset)
            .readTo(&r, sizeof(typename T::Type));
        return r;
    }
};
