#pragma once

#include <karm-base/clamp.h>

#include "traits.h"
#include "types.h"

namespace Karm::Io {

struct Sink {
    auto write(void const *, size_t size) -> Base::Result<size_t> { return size; }
};

struct Zero {
    auto read(void *data, size_t size) -> Base::Result<size_t> {
        memset(data, 0, size);
        return size;
    }
};

struct Repeat {
    uint8_t _byte;

    Repeat(uint8_t byte) : _byte(byte) {}

    auto read(void *data, size_t size) -> Base::Result<size_t> {
        memset(data, _byte, size);
        return size;
    }
};

struct Empty {
    auto read(void *, size_t) -> Base::Result<size_t> { return 0; }
};

template <Reader Reader>
struct Limite {
    Reader _reader;
    size_t _limit;
    size_t _read;

    Limite(Reader &&reader, size_t limit)
        : _reader(std::forward<Reader>(reader)),
          _limit(limit) {
    }

    auto read(void *data, size_t size) -> Base::Result<size_t> {
        if (_read + size > _limit) {
            size = _limit - _read;
        }

        size_t read = try$(_reader.read(data, size));
        _read += read;
        return read;
    }
};

template <SeekableWriter Writer>
struct Slice {
    Writer _writer;
    size_t _start;
    size_t _end;

    Slice(Writer writer, size_t size)
        : _writer(std::forward<Writer>(writer)) {
        _start = try$(_writer.tell());
        _end = _start + size;
    }

    auto seek(Seek seek) -> Base::Result<size_t> {
        size_t pos = try$(tell(_writer));
        pos = seek.apply(pos);
        pos = clamp(pos, _start, _end);
        return try$(_writer.seek(Seek::from_begin(pos)));
    }

    auto write(void const *data, size_t size) -> Base::Result<size_t> {
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

} // namespace Karm::Io
