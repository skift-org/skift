#pragma once

#include <karm-base/cursor.h>
#include <karm-base/endiant.h>
#include <karm-base/string.h>

struct BScan {
    Cursor<u8> _cursor;

    BScan(Bytes bytes) : _cursor(bytes) {}

    bool ended() {
        return _cursor.ended();
    }

    usize rem() {
        return _cursor.rem();
    }

    BScan &skip(usize n) {
        n = min(n, rem());
        _cursor.next(n);
        return *this;
    }

    BScan peek(usize n) {
        BScan c{*this};
        c.skip(n);
        return c;
    }

    template <typename T>
    bool readTo(T *buf, usize n) {
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
    bool peekTo(T *buf, usize n) {
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
    T nextBe() {
        _be<T> r{};
        readTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    T nextLe() {
        _le<T> r;
        readTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    T peekBe() {
        _be<T> r{};
        peekTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    T peekLe() {
        _be<T> r;
        peekTo(&r, sizeof(T));
        return r;
    }

    u8 nextU8be() { return nextBe<u8>(); }
    u16 nextU16be() { return nextBe<u16>(); }
    u32 nextU32be() { return nextBe<u32>(); }
    u64 nextU64be() { return nextBe<u64>(); }

    u8 nextU8le() { return nextLe<u8>(); }
    u16 nextU16le() { return nextLe<u16>(); }
    u32 nextU32le() { return nextLe<u32>(); }
    u64 nextU64le() { return nextLe<u64>(); }

    i8 nextI8be() { return nextBe<i8>(); }
    i16 nextI16be() { return nextBe<i16>(); }
    i32 nextI32be() { return nextBe<i32>(); }
    i64 nextI64be() { return nextBe<i64>(); }

    i8 nextI8le() { return nextLe<i8>(); }
    i16 nextI16le() { return nextLe<i16>(); }
    i32 nextI32le() { return nextLe<i32>(); }
    i64 nextI64le() { return nextLe<i64>(); }

    u8 peekU8be() { return peekBe<u8>(); }
    u16 peekU16be() { return peekBe<u16>(); }
    u32 peekU32be() { return peekBe<u32>(); }
    u64 peekU64be() { return peekBe<u64>(); }

    u8 peekU8le() { return peekLe<u8>(); }
    u16 peekU16le() { return peekLe<u16>(); }
    u32 peekU32le() { return peekLe<u32>(); }
    u64 peekU64le() { return peekLe<u64>(); }

    i8 peekI8be() { return peekBe<i8>(); }
    i16 peekI16be() { return peekBe<i16>(); }
    i32 peekI32be() { return peekBe<i32>(); }
    i64 peekI64be() { return peekBe<i64>(); }

    i8 peekI8le() { return peekLe<i8>(); }
    i16 peekI16le() { return peekLe<i16>(); }
    i32 peekI32le() { return peekLe<i32>(); }
    i64 peekI64le() { return peekLe<i64>(); }

    Str nextStr(usize n) {
        n = clamp(n, 0uz, rem());
        Str s{(char const *)_cursor.buf(), n};
        _cursor.next(n);
        return s;
    }

    Str nextCStr() {
        usize n = 0;
        while (n < rem() and _cursor.buf()[n] != '\0') {
            n++;
        }
        return nextStr(n);
    }

    Bytes nextBytes(usize n) {
        n = clamp(n, 0uz, rem());
        Bytes b{_cursor.buf(), n};
        _cursor.next(n);
        return b;
    }
};

struct BChunk {
    Bytes _slice;

    BChunk() = default;

    BChunk(Bytes slice) : _slice(slice) {}

    bool present() const {
        return _slice.len() > 0;
    }

    BScan begin() const {
        return _slice;
    }
};
