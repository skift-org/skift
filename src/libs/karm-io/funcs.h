#pragma once

#include <karm-base/clamp.h>
#include <karm-base/cons.h>
#include <karm-base/rune.h>
#include <karm-base/string.h>

#include "impls.h"
#include "traits.h"

namespace Karm::Io {

/* --- Read ----------------------------------------------------------------- */

inline Res<size_t> pread(Readable auto &reader, MutBytes bytes, Seek seek) {
    auto result = try$(reader.seek(seek));
    return reader.read(bytes);
}

inline Res<Byte> getByte(Readable auto &reader) {
    Byte byte;
    try$(reader.read({&byte, 1}));
    return Ok(byte);
}

inline Res<String> readAllUtf8(Readable auto &reader) {
    StringWriter writer;
    Array<Utf8::Unit, 512> buf;
    while (true) {
        size_t read = try$(reader.read(buf.mutBytes()));
        if (read == 0) {
            break;
        }
        try$(writer.writeUnit({buf.buf(), read}));
    }
    return Ok(writer.take());
}

/* --- Write ---------------------------------------------------------------- */

inline Res<size_t> pwrite(Writable auto &writer, Bytes bytes, Seek seek) {
    auto result = try$(writer.seek(seek));
    return writer.write(bytes);
}

inline Res<size_t> putByte(Writable auto &writer, Byte byte) {
    return writer.write({&byte, 1});
}

/* --- Seek ----------------------------------------------------------------- */

inline Res<size_t> tell(Seekable auto &seeker) {
    return seeker.seek(Seek::fromCurrent(0));
}

inline Res<size_t> size(Seekable auto &seeker) {
    size_t current = try$(tell(seeker));
    size_t end = try$(seeker.seek(Seek::fromEnd(0)));
    try$(seeker.seek(Seek::fromBegin(current)));
    return Ok(end);
}

inline Res<size_t> skip(Seekable auto &seeker, size_t n) {
    return seeker.seek(Seek::fromCurrent(n));
}

inline Res<size_t> skip(Readable auto &reader, size_t n) {
    Sink sink;
    return copy(reader, sink, n);
}

/* --- Copy ----------------------------------------------------------------- */

inline Res<size_t> copy(Readable auto &reader, MutBytes bytes) {
    size_t readed = 0;
    while (readed < bytes.len()) {
        readed += try$(reader.read(next(bytes, readed)));
    }
    return Ok(readed);
}

inline Res<size_t> copy(Readable auto &reader, Writable auto &writer) {
    Array<Byte, 4096> buffer;
    size_t result = 0;
    while (true) {
        auto read = try$(reader.read(mutBytes(buffer)));
        if (read == 0) {
            return Ok(result);
        }

        result += read;

        auto written = try$(writer.write(sub(buffer, 0, read)));
        if (written != read) {
            return Ok(result);
        }
    }
}

inline Res<size_t> copy(Readable auto &reader, Writable auto &writer, size_t size) {
    Array<Bytes, 4096> buffer;
    size_t result = 0;
    while (size > 0) {
        auto read = try$(reader.read(sub(buffer, 0, size)));
        if (read == 0) {
            return Ok(result);
        }

        result += read;

        auto written = try$(writer.write(sub(buffer, 0, read)));
        if (written != read) {
            return Ok(result);
        }
        size -= read;
    }
    return Ok(result);
}

} // namespace Karm::Io
