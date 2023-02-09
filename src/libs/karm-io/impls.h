#pragma once

#include <karm-base/clamp.h>
#include <karm-base/result.h>
#include <karm-base/slice.h>

#include "traits.h"
#include "types.h"

namespace Karm::Io {

struct Sink : public Writer {
    Result<size_t> write(Bytes bytes) override {
        return sizeOf(bytes);
    }
};

struct Zero : public Reader {
    Result<size_t> read(MutBytes bytes) override {
        return zeroFill(bytes);
    }
};

struct Repeat : public Reader {
    Byte _byte;

    Repeat(Byte byte) : _byte(byte) {}

    Result<size_t> read(MutBytes bytes) override {
        return fill(bytes, _byte);
    }
};

struct Empty : public Reader {
    Result<size_t> read(MutBytes)
        override {
        return 0;
    }
};

template <Readable Readable>
struct Limit : public Reader {
    Readable _reader;
    size_t _limit;
    size_t _read;

    Limit(Readable &&reader, size_t limit)
        : _reader(std::forward<Readable>(reader)),
          _limit(limit) {
    }

    Result<size_t> read(MutBytes bytes) override {
        size_t size = clamp(sizeOf(bytes), 0uz, _limit - _read);
        size_t read = try$(_reader.read(bytes.buf(), size));
        _read += read;
        return read;
    }
};

template <SeekableWritable Writable>
struct WriterSlice : public Writer, public Seeker {
    Writable _writer;
    size_t _start;
    size_t _end;

    WriterSlice(Writable writer, size_t start, size_t end) : _writer(writer), _start(start), _end(end) {}

    Result<size_t> seek(Seek seek) override {
        size_t pos = try$(tell(_writer));
        size_t s = size(*this);
        pos = seek.apply(pos, s);
        pos = clamp(pos, _start, _end);
        return try$(_writer.seek(Seek::fromBegin(pos)));
    }

    Result<size_t> write(Bytes bytes) override {
        size_t pos = try$(tell(_writer));

        if (pos < _start) {
            try$(_writer.seek(Seek::fromBegin(_start)));
        }

        if (pos > _end) {
            return 0;
        }

        size_t size = clamp(sizeOf(bytes), 0uz, _end - pos);
        return try$(_writer.write(sub(bytes, 0, size)));
    }
};

template <SeekableWritable Writable>
inline Result<Slice<Writable>>
makeSlice(Writable &&writer, size_t size) {
    auto start = try$(writer.tell());
    auto end = start + size;

    return Slice{std::forward<Writable>(writer), start, end};
}

struct BufReader :
    public Reader,
    public Seeker {
    Bytes _buf;
    size_t _pos;

    BufReader(Bytes buf) : _buf(buf), _pos(0) {}

    Result<size_t> read(MutBytes bytes) override {
        Bytes slice = sub(_buf, _pos, sizeOf(bytes));
        size_t read = copy(slice, bytes);
        _pos += read;
        return read;
    }

    Result<size_t> seek(Seek seek) override {
        _pos = seek.apply(_pos, sizeOf(_buf));
        _pos = clamp(_pos, 0uz, sizeOf(_buf));
        return _pos;
    }
};

struct BufWriter :
    public Writer,
    public Seeker {
    MutBytes _buf;
    size_t _pos;

    BufWriter(MutBytes buf) : _buf(buf), _pos(0) {}

    Result<size_t> seek(Seek seek) override {
        _pos = seek.apply(_pos, sizeOf(_buf));
        _pos = clamp(_pos, 0uz, sizeOf(_buf));
        return _pos;
    }

    Result<size_t> write(Bytes bytes) override {
        MutBytes slice = mutSub(_buf, _pos, sizeOf(bytes));
        size_t written = copy(bytes, slice);
        _pos += written;
        return written;
    }
};

struct BitReader {
    Reader &_reader;
    uint8_t _bits;
    uint8_t _len;

    BitReader(Reader &reader)
        : _reader(reader), _bits(0), _len(0) {}

    Result<uint8_t> readBit() {
        if (_len == 0) {
            try$(_reader.read(MutBytes{&_bits, 1}));
            _len = 8;
        }

        uint8_t bit = _bits & 1;
        _bits >>= 1;
        _len -= 1;

        return bit;
    }

    template <typename T>
    Result<T> readBits(size_t len) {
        T bits = 0;
        for (size_t i = 0; i < len; i++) {
            bits |= try$(readBit()) << i;
        }
        return bits;
    }
};

template <StaticEncoding E>
struct _StringWriter : public _TextWriter {
    Buf<typename E::Unit> _buf{};

    _StringWriter(size_t cap = 16) : _buf(cap) {}

    Result<size_t> write(Bytes)
        override {
        panic("can't write raw bytes to a string");
    }

    Result<size_t> writeStr(Str str) override {
        size_t written = 0;
        for (auto rune : iterRunes(str)) {
            written += try$(writeRune(rune));
        }
        return written;
    }

    Result<size_t> writeRune(Rune rune) override {
        typename E::One one;
        if (!E::encodeUnit(rune, one)) {
            return 0;
        }

        for (auto unit : iter(one)) {
            _buf.insert(_buf.len(), std::move(unit));
        }

        return 1;
    }

    String finalize() {
        size_t len = _buf.size();
        _buf.insert(len, 0);

        return String{MOVE, _buf.take(), len};
    }
};

using StringWriter = _StringWriter<Utf8>;

} // namespace Karm::Io
