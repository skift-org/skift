#pragma once

#include <karm-base/clamp.h>
#include <karm-base/result.h>
#include <karm-base/slice.h>

#include "traits.h"
#include "types.h"

namespace Karm::Io {

struct Sink : public Writer {
    Result<size_t> write(Bytes bytes) override {
        return bytes.size();
    }
};

struct Zero : public Reader {
    Result<size_t> read(MutBytes bytes) override {
        return bytes.zero();
    }
};

struct Repeat : public Reader {
    uint8_t _byte;

    Repeat(uint8_t byte) : _byte(byte) {}

    Result<size_t> read(MutBytes bytes) override {
        return bytes.fill((Byte)_byte);
    }
};

struct Empty : public Reader {
    Result<size_t> read(MutBytes) override {
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
        size_t size = clamp(bytes.size(), 0uz, _limit - _read);
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

        size_t size = clamp(bytes.size(), 0uz, _end - pos);
        return try$(_writer.write(bytes.sub(0, size)));
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
        Bytes slice = _buf.sub(_pos, bytes.size());
        size_t read = slice.copyTo(bytes);
        _pos += read;
        return read;
    }

    Result<size_t> seek(Seek seek) override {
        _pos = seek.apply(_pos, _buf.size());
        _pos = clamp(_pos, 0uz, _buf.size());
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
        _pos = seek.apply(_pos, _buf.size());
        _pos = clamp(_pos, 0uz, _buf.size());
        return _pos;
    }

    Result<size_t> write(Bytes bytes) override {
        MutBytes slice = _buf.sub(_pos, bytes.size());
        size_t written = bytes.copyTo(slice);
        _pos += written;
        return written;
    }
};

} // namespace Karm::Io
