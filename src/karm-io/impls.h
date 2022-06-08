#pragma once

#include <karm-base/bytes.h>
#include <karm-base/clamp.h>
#include <karm-base/result.h>
#include <karm-base/slice.h>

#include "traits.h"
#include "types.h"

namespace Karm::Io {

struct Sink : public Writer {
    Result<size_t> write(void const *, size_t size) override { return size; }
};

struct Zero : public Reader {
    Result<size_t> read(void *data, size_t size) override {
        memset(data, 0, size);
        return size;
    }
};

struct Repeat : public Reader {
    uint8_t _byte;

    Repeat(uint8_t byte) : _byte(byte) {}

    Result<size_t> read(void *data, size_t size) override {
        memset(data, _byte, size);
        return size;
    }
};

struct Empty : public Reader {
    Result<size_t> read(void *, size_t) override {
        return 0;
    }
};

template <Readable Readable>
struct Limite : public Reader {
    Readable _reader;
    size_t _limit;
    size_t _read;

    Limite(Readable &&reader, size_t limit)
        : _reader(std::forward<Readable>(reader)),
          _limit(limit) {
    }

    Result<size_t> read(void *data, size_t size) override {
        if (_read + size > _limit) {
            size = _limit - _read;
        }

        size_t read = try$(_reader.read(data, size));
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

    Result<size_t> write(void const *data, size_t size) override {
        size_t pos = try$(tell(_writer));

        if (pos < _start) {
            return 0;
        }

        if (pos + size > _end) {
            size = _end - pos;
        }

        return try$(_writer.write(data, size));
    }
};

struct BufReader :
    public Reader,
    public Seeker {
    Bytes _buf;
    size_t _pos;

    BufReader(Bytes buf) : _buf(buf), _pos(0) {}

    BufReader(void *buf, size_t size) : _buf(buf, size), _pos(0) {}

    Result<size_t> read(void *data, size_t size) override {
        size_t read = min(size, _buf.len() - _pos);
        memcpy(data, _buf.buf() + _pos, read);
        _pos += read;
        return read;
    }

    Result<size_t> seek(Seek seek) override {
        _pos = seek.apply(_pos, _buf.len());
        _pos = clamp(_pos, 0uz, _buf.len());
        return _pos;
    }
};

struct BufWriter :
    public Writer,
    public Seeker {
    Bytes _buf;
    size_t _pos;

    BufWriter(Bytes buf) : _buf(buf), _pos(0) {}

    BufWriter(void *buf, size_t len) : _buf(buf, len), _pos(0) {}

    Result<size_t> seek(Seek seek) override {
        _pos = seek.apply(_pos, _buf.len());
        _pos = clamp(_pos, 0uz, _buf.len());
        return _pos;
    }

    Result<size_t> write(void const *data, size_t size) override {
        size_t write = min(size, _buf.len() - _pos);
        memcpy((void *)(_buf.buf() + _pos), data, write);
        _pos += write;
        return write;
    }
};

template <SeekableWritable Writable>
static inline Result<Slice<Writable>> makeSlice(Writable &&writer, size_t size) {
    auto start = try$(writer.tell());
    auto end = start + size;

    return Slice{std::forward<Writable>(writer), start, end};
}

} // namespace Karm::Io
