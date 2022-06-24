#pragma once

#include <karm-base/clamp.h>
#include <karm-base/cons.h>
#include <karm-base/rune.h>
#include <karm-base/string.h>

#include "impls.h"
#include "traits.h"

namespace Karm::Io {

/* --- Read ----------------------------------------------------------------- */

static inline Result<size_t> pread(Readable auto &reader, MutBytes bytes, Seek seek) {
    auto result = try$(reader.seek(seek));
    return try$(reader.read(bytes));
}

static inline Result<Byte> getByte(Readable auto &reader) {
    Byte byte;
    try$(reader.read({&byte, 1}));
    return byte;
}

/* --- Write ---------------------------------------------------------------- */

static inline Result<size_t> pwrite(Writable auto &writer, Bytes bytes, Seek seek) {
    auto result = try$(writer.seek(seek));
    return try$(writer.write(bytes));
}

static inline Result<size_t> putByte(Writable auto &writer, Byte byte) {
    return writer.write({&byte, 1});
}

/* --- Seek ----------------------------------------------------------------- */

static inline Result<size_t> tell(Seekable auto &seeker) {
    return try$(seeker.seek(Seek::fromCurrent(0)));
}

static inline Result<size_t> size(Seekable auto &seeker) {
    size_t current = try$(tell(seeker));
    size_t end = try$(seeker.seek(Seek::fromEnd(0)));
    try$(seeker.seek(Seek::fromBegin(current)));
    return end;
}

static inline Result<size_t> skip(Seekable auto &seeker, size_t n) {
    return try$(seeker.seek(Seek::fromCurrent(n)));
}

static inline Result<size_t> skip(Readable auto &reader, size_t n) {
    Sink sink;
    return copy(reader, sink, n);
}

/* --- Copy ----------------------------------------------------------------- */

static inline Result<size_t> copy(Readable auto &reader, MutBytes bytes) {
    size_t readed = 0;
    while (readed < bytes.len()) {
        readed += try$(reader.read(bytes.sub(readed)));
    }

    return readed;
}

static inline Result<size_t> copy(Readable auto &reader, Writable auto &writer) {
    Array<Byte, 4096> buffer;
    size_t result = 0;
    while (true) {
        auto read = try$(reader.read(buffer.mutBytes()));

        if (read == 0) {
            return result;
        }

        result += read;
        auto written = try$(writer.write(buffer.sub(0, read)));

        if (written != read) {
            return result;
        }
    }
}

static inline Result<size_t> copy(Readable auto &reader, Writable auto &writer, size_t size) {
    Array<Bytes, 4096> buffer;
    size_t result = 0;
    while (size > 0) {
        auto read = try$(reader.read(buffer.sub(0, size)));
        if (read == 0) {
            break;
        }
        result += read;
        auto written = try$(writer.write(buffer.sub(0, read)));
        if (written != read) {
            return result;
        }
        size -= read;
    }
    return result;
}

} // namespace Karm::Io
