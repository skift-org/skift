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

    always_inline constexpr BScan(Bytes bytes)
        : _start(bytes),
          _cursor(bytes) {}

    always_inline constexpr void rewind() {
        _cursor = _start;
    }

    always_inline constexpr BScan& seek(usize n) {
        rewind();
        return skip(n);
    }

    always_inline constexpr usize tell() {
        return _cursor - _start;
    }

    always_inline constexpr bool ended() {
        return _cursor.ended();
    }

    always_inline constexpr usize rem() {
        return _cursor.rem();
    }

    always_inline constexpr Bytes remBytes() {
        return {_cursor.buf(), rem()};
    }

    always_inline constexpr BScan& skip(usize n) {
        n = min(n, rem());
        _cursor.next(n);
        return *this;
    }

    always_inline constexpr BScan peek(usize n) {
        BScan c{*this};
        c.skip(n);
        return c;
    }

    template <typename T>
    always_inline constexpr bool readTo(T* buf, usize n = 1) {
        if (rem() < n) {
            return false;
        }

        u8* b = reinterpret_cast<u8*>(buf);
        for (usize i = 0; i < sizeof(T) * n; i++) {
            b[i] = _cursor.next();
        }
        return true;
    }

    template <typename T>
    always_inline constexpr bool peekTo(T* buf, usize n = 1) {
        if (rem() < n)
            return false;

        u8* b = reinterpret_cast<u8*>(buf);
        for (usize i = 0; i < sizeof(T) * n; i++)
            b[i] = _cursor.buf()[i];

        return true;
    }

    template <typename T>
    always_inline constexpr T nextBe() {
        Be<T> r{};
        readTo(&r);
        return r;
    }

    template <typename T>
    always_inline constexpr T nextLe() {
        Le<T> r;
        readTo(&r);
        return r;
    }

    template <typename T>
    always_inline constexpr T peekBe() {
        Be<T> r{};
        peekTo(&r);
        return r;
    }

    template <typename T>
    always_inline constexpr T peekLe() {
        Le<T> r;
        peekTo(&r);
        return r;
    }

    always_inline constexpr void alignBits() {
        _bitsLen = 0;
    }

    /// Read bits in most significant bit first order.
    always_inline constexpr u8 nextBitbe() {
        if (_bitsLen == 0) {
            _bits = nextU8be();
            _bitsLen = 8;
        }
        u8 r = _bits >> 7;
        _bits <<= 1;
        _bitsLen--;
        return r;
    }

    always_inline constexpr u8 peekBitbe() {
        BScan c{*this};
        return c.nextBitbe();
    }

    always_inline constexpr usize nextBitsbe(usize n) {
        usize r = 0;
        for (usize i = n - 1; i < n; i--) {
            r |= nextBitbe() << i;
        }
        return r;
    }

    always_inline constexpr usize peekBitsbe(usize n) {
        BScan c{*this};
        return c.nextBitsbe(n);
    }

    always_inline constexpr void skipBitsbe(usize n) {
        for (usize i = 0; i < n; i++) {
            nextBitbe();
        }
    }

    /// Read bits in least significant bit first order.
    always_inline constexpr u8 nextBitle() {
        if (_bitsLen == 0) {
            _bits = nextU8le();
            _bitsLen = 8;
        }
        u8 r = _bits & 1;
        _bits >>= 1;
        _bitsLen--;
        return r;
    }

    always_inline constexpr u8 peekBitle() {
        BScan c{*this};
        return c.nextBitle();
    }

    always_inline constexpr usize nextBitsle(usize n) {
        usize r = 0;
        for (usize i = 0; i < n; i++) {
            r |= nextBitle() << i;
        }
        return r;
    }

    always_inline constexpr usize peekBitsle(usize n) {
        BScan c{*this};
        return c.nextBitsle(n);
    }

    always_inline constexpr void skipBitsle(usize n) {
        for (usize i = 0; i < n; i++) {
            nextBitle();
        }
    }

    always_inline constexpr u8 nextU8be() { return nextBe<u8>(); }

    always_inline constexpr u16 nextU16be() { return nextBe<u16>(); }

    always_inline constexpr u32 nextU32be() { return nextBe<u32>(); }

    always_inline constexpr u64 nextU64be() { return nextBe<u64>(); }

    always_inline constexpr u8 nextU8le() { return nextLe<u8>(); }

    always_inline constexpr u16 nextU16le() { return nextLe<u16>(); }

    always_inline constexpr u32 nextU32le() { return nextLe<u32>(); }

    always_inline constexpr u64 nextU64le() { return nextLe<u64>(); }

    always_inline constexpr i8 nextI8be() { return nextBe<i8>(); }

    always_inline constexpr i16 nextI16be() { return nextBe<i16>(); }

    always_inline constexpr i32 nextI32be() { return nextBe<i32>(); }

    always_inline constexpr i64 nextI64be() { return nextBe<i64>(); }

    always_inline constexpr i8 nextI8le() { return nextLe<i8>(); }

    always_inline constexpr i16 nextI16le() { return nextLe<i16>(); }

    always_inline constexpr i32 nextI32le() { return nextLe<i32>(); }

    always_inline constexpr i64 nextI64le() { return nextLe<i64>(); }

    always_inline constexpr u8 peekU8be() { return peekBe<u8>(); }

    always_inline constexpr u16 peekU16be() { return peekBe<u16>(); }

    always_inline constexpr u32 peekU32be() { return peekBe<u32>(); }

    always_inline constexpr u64 peekU64be() { return peekBe<u64>(); }

    always_inline constexpr u8 peekU8le() { return peekLe<u8>(); }

    always_inline constexpr u16 peekU16le() { return peekLe<u16>(); }

    always_inline constexpr u32 peekU32le() { return peekLe<u32>(); }

    always_inline constexpr u64 peekU64le() { return peekLe<u64>(); }

    always_inline constexpr i8 peekI8be() { return peekBe<i8>(); }

    always_inline constexpr i16 peekI16be() { return peekBe<i16>(); }

    always_inline constexpr i32 peekI32be() { return peekBe<i32>(); }

    always_inline constexpr i64 peekI64be() { return peekBe<i64>(); }

    always_inline constexpr i8 peekI8le() { return peekLe<i8>(); }

    always_inline constexpr i16 peekI16le() { return peekLe<i16>(); }

    always_inline constexpr i32 peekI32le() { return peekLe<i32>(); }

    always_inline constexpr i64 peekI64le() { return peekLe<i64>(); }

    always_inline constexpr Str nextStr(usize n) {
        n = clamp(n, 0uz, rem());
        Str s{(char const*)_cursor.buf(), n};
        _cursor.next(n);
        return s;
    }

    always_inline constexpr Str nextCStr() {
        usize n = 0;
        while (n < rem() and _cursor.buf()[n] != '\0') {
            n++;
        }
        return nextStr(n);
    }

    always_inline constexpr Bytes nextBytes(usize n) {
        n = clamp(n, 0uz, rem());
        Bytes b{_cursor.buf(), n};
        _cursor.next(n);
        return b;
    }

    always_inline constexpr bool copyTo(MutBytes dst) {
        if (dst.len() < rem()) {
            return false;
        }

        for (usize i = 0; i < rem(); i++) {
            dst[i] = _cursor.buf()[i];
        }
        return true;
    }
};

template <typename T, usize Offset>
struct BField {
    using Type = T;
    static constexpr usize offset = Offset;
};

struct BChunk {
    Bytes _slice{};

    always_inline constexpr BChunk() = default;

    always_inline constexpr BChunk(Bytes slice) : _slice(slice) {}

    always_inline constexpr bool present() const {
        return _slice.len() > 0;
    }

    always_inline constexpr BScan begin() const {
        return _slice;
    }

    always_inline constexpr Bytes bytes() const {
        return _slice;
    }

    template <typename T>
    always_inline constexpr typename T::Type get() const {
        typename T::Type r{};
        begin()
            .skip(T::offset)
            .readTo(&r);
        return r;
    }
};

struct BEmit {
    Io::Writer& _writer;

    always_inline constexpr BEmit(Io::Writer& writer)
        : _writer(writer) {}

    template <Meta::TrivialyCopyable T>
    always_inline constexpr void writeFrom(T const& v) {
        (void)_writer.write(Bytes{(Byte const*)&v, sizeof(v)});
    }

    always_inline constexpr void writeU8be(u8be v) {
        writeFrom(v);
    }

    always_inline constexpr void writeU16be(u16be v) {
        writeFrom(v);
    }

    always_inline constexpr void writeU32be(u32be v) {
        writeFrom(v);
    }

    always_inline constexpr void writeU64be(u64be v) {
        writeFrom(v);
    }

    always_inline constexpr void writeU8le(u8le v) {
        writeFrom(v);
    }

    always_inline constexpr void writeU16le(u16le v) {
        writeFrom(v);
    }

    always_inline constexpr void writeU32le(u32le v) {
        writeFrom(v);
    }

    always_inline constexpr void writeU64le(u64le v) {
        writeFrom(v);
    }

    always_inline constexpr void writeI8be(i8be v) {
        writeFrom(v);
    }

    always_inline constexpr void writeI16be(i16be v) {
        writeFrom(v);
    }

    always_inline constexpr void writeI32be(i32be v) {
        writeFrom(v);
    }

    always_inline constexpr void writeI64be(i64be v) {
        writeFrom(v);
    }

    always_inline constexpr void writeI8le(i8le v) {
        writeFrom(v);
    }

    always_inline constexpr void writeI16le(i16le v) {
        writeFrom(v);
    }

    always_inline constexpr void writeI32le(i32le v) {
        writeFrom(v);
    }

    always_inline constexpr void writeI64le(i64le v) {
        writeFrom(v);
    }

    always_inline constexpr void writeStr(Str s) {
        (void)_writer.write(Bytes{(Byte const*)s.buf(), s.len()});
    }

    always_inline constexpr void writeCStr(Str s) {
        (void)_writer.write(Bytes{(Byte const*)s.buf(), s.len()});
        (void)_writer.write(Bytes{(Byte const*)"\0", 1});
    }

    always_inline constexpr void writeBytes(Bytes b) {
        (void)_writer.write(b);
    }
};

} // namespace Karm::Io
