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
static inline Result<Slice<Writable>>
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

struct StringWriter {
};

} // namespace Karm::Io
