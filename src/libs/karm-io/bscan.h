#pragma once

#include <karm-base/cursor.h>
#include <karm-base/endian.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

namespace Karm::Io {

struct BScan {
    Cursor<u8> _start;
    Cursor<u8> _cursor;
    u8 _bits = 0;
    u8 _bitsLen = 0;

    ALWAYS_INLINE constexpr BScan(Bytes bytes)
        : _start(bytes),
          _cursor(bytes) {}

    ALWAYS_INLINE constexpr void rewind() {
        _cursor = _start;
    }

    ALWAYS_INLINE constexpr BScan &seek(usize n) {
        rewind();
        return skip(n);
    }

    ALWAYS_INLINE constexpr usize tell() {
        return _cursor - _start;
    }

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
        Be<T> r{};
        readTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    ALWAYS_INLINE constexpr T nextLe() {
        Le<T> r;
        readTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    ALWAYS_INLINE constexpr T peekBe() {
        Be<T> r{};
        peekTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    ALWAYS_INLINE constexpr T peekLe() {
        Le<T> r;
        peekTo(&r, sizeof(T));
        return r;
    }

    ALWAYS_INLINE constexpr void alignBits() {
        _bitsLen = 0;
    }

    /// Read bits in most significant bit first order.
    ALWAYS_INLINE constexpr u8 nextBitbe() {
        if (_bitsLen == 0) {
            _bits = nextU8be();
            _bitsLen = 8;
        }
        u8 r = _bits >> 7;
        _bits <<= 1;
        _bitsLen--;
        return r;
    }

    ALWAYS_INLINE constexpr u8 peekBitbe() {
        BScan c{*this};
        return c.nextBitbe();
    }

    ALWAYS_INLINE constexpr usize nextBitsbe(usize n) {
        usize r = 0;
        for (usize i = 0; i < n; i++) {
            r |= nextBitbe() << i;
        }
        return r;
    }

    ALWAYS_INLINE constexpr usize peekBitsbe(usize n) {
        BScan c{*this};
        return c.nextBitsbe(n);
    }

    ALWAYS_INLINE constexpr void skipBitsbe(usize n) {
        for (usize i = 0; i < n; i++) {
            nextBitbe();
        }
    }

    /// Read bits in least significant bit first order.
    ALWAYS_INLINE constexpr u8 nextBitle() {
        if (_bitsLen == 0) {
            _bits = nextU8le();
            _bitsLen = 8;
        }
        u8 r = _bits & 1;
        _bits >>= 1;
        _bitsLen--;
        return r;
    }

    ALWAYS_INLINE constexpr u8 peekBitle() {
        BScan c{*this};
        return c.nextBitle();
    }

    ALWAYS_INLINE constexpr usize nextBitsle(usize n) {
        usize r = 0;
        for (usize i = 0; i < n; i++) {
            r |= nextBitle() << i;
        }
        return r;
    }

    ALWAYS_INLINE constexpr usize peekBitsle(usize n) {
        BScan c{*this};
        return c.nextBitsle(n);
    }

    ALWAYS_INLINE constexpr void skipBitsle(usize n) {
        for (usize i = 0; i < n; i++) {
            nextBitle();
        }
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

    ALWAYS_INLINE constexpr bool copyTo(MutBytes dst) {
        if (dst.len() < rem()) {
            return false;
        }

        for (usize i = 0; i < rem(); i++) {
            dst[i] = _cursor.buf()[i];
        }
        return true;
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
    Bytes _slice{};

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

struct BEmit {
    Io::Writer &_writer;

    ALWAYS_INLINE constexpr BEmit(Io::Writer &writer)
        : _writer(writer) {}

    ALWAYS_INLINE constexpr void writeU8be(u8be v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeU16be(u16be v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeU32be(u32be v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeU64be(u64be v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeU8le(u8le v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeU16le(u16le v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeU32le(u32le v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeU64le(u64le v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeI8be(i8be v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeI16be(i16be v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeI32be(i32be v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeI64be(i64be v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeI8le(i8le v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeI16le(i16le v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeI32le(i32le v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeI64le(i64le v) {
        (void)_writer.write(Bytes{(Byte const *)&v, sizeof(v)});
    }

    ALWAYS_INLINE constexpr void writeStr(Str s) {
        (void)_writer.write(Bytes{(Byte const *)s.buf(), s.len()});
    }

    ALWAYS_INLINE constexpr void writeCStr(Str s) {
        (void)_writer.write(Bytes{(Byte const *)s.buf(), s.len()});
        (void)_writer.write(Bytes{(Byte const *)"\0", 1});
    }

    ALWAYS_INLINE constexpr void writeBytes(Bytes b) {
        (void)_writer.write(b);
    }
};

} // namespace Karm::Io
