#pragma once

#include <karm-base/cursor.h>
#include <karm-base/endiant.h>
#include <karm-base/string.h>

struct BScan {
    Cursor<uint8_t> _cursor;

    BScan(Bytes bytes) : _cursor(bytes) {}

    bool ended() {
        return _cursor.ended();
    }

    size_t rem() {
        return _cursor.rem();
    }

    BScan &skip(size_t n) {
        n = min(n, rem());
        _cursor.next(n);
        return *this;
    }

    BScan peek(size_t n) {
        BScan c{*this};
        c.skip(n);
        return c;
    }

    template <typename T>
    bool readTo(T *buf, size_t n) {
        if (rem() < n) {
            return false;
        }

        uint8_t *b = reinterpret_cast<uint8_t *>(buf);
        for (size_t i = 0; i < n; i++) {
            b[i] = _cursor.next();
        }
        return true;
    }

    template <typename T>
    bool peekTo(T *buf, size_t n) {
        if (rem() < n) {
            return false;
        }

        uint8_t *b = reinterpret_cast<uint8_t *>(buf);
        for (size_t i = 0; i < n; i++) {
            b[i] = _cursor.buf()[i];
        }
        return true;
    }

    template <typename T>
    T nextBe() {
        _Be<T> r{};
        readTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    T nextLe() {
        _Le<T> r;
        readTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    T peekBe() {
        _Be<T> r{};
        peekTo(&r, sizeof(T));
        return r;
    }

    template <typename T>
    T peekLe() {
        _Le<T> r;
        peekTo(&r, sizeof(T));
        return r;
    }

    uint8_t nextBeUint8() { return nextBe<uint8_t>(); }
    uint16_t nextBeUint16() { return nextBe<uint16_t>(); }
    uint32_t nextBeUint32() { return nextBe<uint32_t>(); }
    uint64_t nextBeUint64() { return nextBe<uint64_t>(); }

    uint8_t nextLeUint8() { return nextLe<uint8_t>(); }
    uint16_t nextLeUint16() { return nextLe<uint16_t>(); }
    uint32_t nextLeUint32() { return nextLe<uint32_t>(); }
    uint64_t nextLeUint64() { return nextLe<uint64_t>(); }

    int8_t nextBeInt8() { return nextBe<int8_t>(); }
    int16_t nextBeInt16() { return nextBe<int16_t>(); }
    int32_t nextBeInt32() { return nextBe<int32_t>(); }
    int64_t nextBeInt64() { return nextBe<int64_t>(); }

    int8_t nextLeInt8() { return nextLe<int8_t>(); }
    int16_t nextLeInt16() { return nextLe<int16_t>(); }
    int32_t nextLeInt32() { return nextLe<int32_t>(); }
    int64_t nextLeInt64() { return nextLe<int64_t>(); }

    uint8_t peekBeUint8() { return peekBe<uint8_t>(); }
    uint16_t peekBeUint16() { return peekBe<uint16_t>(); }
    uint32_t peekBeUint32() { return peekBe<uint32_t>(); }
    uint64_t peekBeUint64() { return peekBe<uint64_t>(); }

    uint8_t peekLeUint8() { return peekLe<uint8_t>(); }
    uint16_t peekLeUint16() { return peekLe<uint16_t>(); }
    uint32_t peekLeUint32() { return peekLe<uint32_t>(); }
    uint64_t peekLeUint64() { return peekLe<uint64_t>(); }

    int8_t peekBeInt8() { return peekBe<int8_t>(); }
    int16_t peekBeInt16() { return peekBe<int16_t>(); }
    int32_t peekBeInt32() { return peekBe<int32_t>(); }
    int64_t peekBeInt64() { return peekBe<int64_t>(); }

    int8_t peekLeInt8() { return peekLe<int8_t>(); }
    int16_t peekLeInt16() { return peekLe<int16_t>(); }
    int32_t peekLeInt32() { return peekLe<int32_t>(); }
    int64_t peekLeInt64() { return peekLe<int64_t>(); }

    Str nextStr(size_t n) {
        n = clamp(n, 0uz, rem());
        Str s{(char const *)_cursor.buf(), n};
        _cursor.next(n);
        return s;
    }

    Str nextCStr() {
        size_t n = 0;
        while (n < rem() && _cursor.buf()[n] != '\0') {
            n++;
        }
        return nextStr(n);
    }

    Bytes nextBytes(size_t n) {
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
