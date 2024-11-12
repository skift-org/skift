#pragma once

#include <karm-base/clamp.h>
#include <karm-base/res.h>
#include <karm-base/slice.h>

#include "traits.h"
#include "types.h"

namespace Karm::Io {

struct Sink : public Writer {
    Res<usize> write(Bytes bytes) override {
        return Ok(sizeOf(bytes));
    }
};

struct Zero : public Reader {
    Res<usize> read(MutBytes bytes) override {
        return Ok(zeroFill(bytes));
    }
};

struct Repeat : public Reader {
    Byte _byte;

    Repeat(Byte byte) : _byte(byte) {}

    Res<usize> read(MutBytes bytes) override {
        return Ok(fill(bytes, _byte));
    }
};

struct Empty : public Reader {
    Res<usize> read(MutBytes) override {
        return Ok(0uz);
    }
};

template <Readable Readable>
struct Limit : public Reader {
    Readable _reader;
    usize _limit;
    usize _read;

    Limit(Readable &&reader, usize limit)
        : _reader(std::forward<Readable>(reader)),
          _limit(limit) {}

    Res<usize> read(MutBytes bytes) override {
        usize size = clamp(sizeOf(bytes), 0uz, _limit - _read);
        usize read = try$(_reader.read(bytes.buf(), size));
        _read += read;
        return Ok(read);
    }
};

template <SeekableWritable Writable>
struct WriterSlice : public Writer, public Seeker {
    Writable _writer;
    usize _start;
    usize _end;

    WriterSlice(Writable writer, usize start, usize end)
        : _writer(writer), _start(start), _end(end) {}

    Res<usize> seek(Seek seek) override {
        usize pos = try$(tell(_writer));
        usize s = size(*this);
        pos = seek.apply(pos, s);
        pos = clamp(pos, _start, _end);
        return _writer.seek(Seek::fromBegin(pos));
    }

    Res<usize> write(Bytes bytes) override {
        usize pos = try$(tell(_writer));

        if (pos < _start) {
            try$(_writer.seek(Seek::fromBegin(_start)));
        }

        if (pos > _end) {
            return Ok(0uz);
        }

        usize size = clamp(sizeOf(bytes), 0uz, _end - pos);
        return _writer.write(sub(bytes, 0, size));
    }
};

template <SeekableWritable Writable>
inline Res<Slice<Writable>> makeSlice(Writable &&writer, usize size) {
    auto start = try$(writer.tell());
    auto end = start + size;

    return Slice{std::forward<Writable>(writer), start, end};
}

struct BufReader :
    public Reader,
    public Seeker {

    Bytes _buf;
    usize _pos;

    BufReader(Bytes buf) : _buf(buf), _pos(0) {}

    Res<usize> read(MutBytes bytes) override {
        Bytes slice = sub(_buf, _pos, _pos + sizeOf(bytes));
        usize read = copy(slice, bytes);
        _pos += read;
        return Ok(read);
    }

    Res<usize> seek(Seek seek) override {
        _pos = seek.apply(_pos, sizeOf(_buf));
        _pos = clamp(_pos, 0uz, sizeOf(_buf));
        return Ok(_pos);
    }
};

struct BufWriter :
    public Writer,
    public Seeker {

    MutBytes _buf;
    usize _pos = 0;

    BufWriter(MutBytes buf) : _buf(buf) {}

    Res<usize> seek(Seek seek) override {
        _pos = seek.apply(_pos, sizeOf(_buf));
        _pos = clamp(_pos, 0uz, sizeOf(_buf));
        return Ok(_pos);
    }

    Res<usize> write(Bytes bytes) override {
        MutBytes slice = mutNext(_buf, _pos);
        usize written = copy(bytes, slice);
        _pos += written;
        return Ok(written);
    }
};

struct BufferWriter : public Writer, public Flusher {
    Buf<Byte> _buf;

    BufferWriter(usize cap = 16) : _buf(cap) {}

    Res<usize> write(Bytes bytes) override {
        _buf.insert(COPY, _buf.len(), bytes.buf(), bytes.len());
        return Ok(bytes.len());
    }

    Bytes bytes() const {
        return _buf;
    }

    Res<usize> flush() override {
        auto l = _buf.len();
        _buf.trunc(0);
        return Ok(l);
    }

    Buf<Byte> take() {
        return std::move(_buf);
    }

    void clear() {
        _buf.trunc(0);
    }
};

struct BitReader {
    Reader &_reader;
    u8 _bits{};
    u8 _len{};

    BitReader(Reader &reader)
        : _reader(reader) {
    }

    Res<u8> readBit() {
        if (_len == 0) {
            try$(_reader.read(MutBytes{&_bits, 1}));
            _len = 8;
        }

        u8 bit = _bits & 1;
        _bits >>= 1;
        _len -= 1;

        return Ok(bit);
    }

    template <typename T>
    Res<T> readBits(usize len) {
        T bits = 0;
        for (usize i = 0; i < len; i++) {
            bits |= try$(readBit()) << i;
        }
        return Ok(bits);
    }
};

template <StaticEncoding E>
struct _StringWriter :
    public TextWriter,
    public _StringBuilder<E> {

    _StringWriter(usize cap = 16) : _StringBuilder<E>(cap) {}

    Res<usize> write(Bytes) override {
        panic("can't write raw bytes to a string");
    }

    Res<usize> writeRune(Rune rune) override {
        _StringBuilder<E>::append(rune);
        return Ok(E::runeLen(rune));
    }

    Res<usize> writeUnit(Slice<typename E::Unit> unit) {
        _StringBuilder<E>::append(unit);
        return Ok(unit.len());
    }
};

using StringWriter = _StringWriter<Utf8>;

} // namespace Karm::Io
