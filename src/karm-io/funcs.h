#pragma once

#include <karm-base/clamp.h>

#include "impls.h"
#include "traits.h"

namespace Karm::Io {

/* --- Read ----------------------------------------------------------------- */

Base::Result<size_t> pread(Readable auto &reader, void *data, size_t size, Seek seek) {
    auto result = try$(reader.seek(seek));
    return try$(reader.read(data, size));
}

Base::Result<uint8_t> getc(Readable auto &reader) {
    uint8_t byte;
    try$(reader.read(&byte, 1));
    return byte;
}

/* --- Write ---------------------------------------------------------------- */

Base::Result<size_t> pwrite(Writable auto &writer, void const *data, size_t size, Seek seek) {
    auto result = try$(writer.seek(seek));
    return try$(writer.write(data, size));
}

Base::Result<uint8_t> putc(Writable auto &writer, uint8_t byte) {
    try$(writer.write(&byte, 1));
    return byte;
}

/* --- Seek ----------------------------------------------------------------- */

Base::Result<size_t> tell(Seekable auto &seeker) {
    return try$(seeker.seek(Seek::fromCurrent(0)));
}

Base::Result<size_t> size(Seekable auto &seeker) {
    size_t current = try$(tell(seeker));
    size_t end = try$(seeker.seek(Seek::fromEnd(0)));
    try$(seeker.seek(Seek::fromBegin(current)));
    return end;
}

Base::Result<size_t> skip(Seekable auto &seeker, size_t n) {
    return try$(seeker.seek(Seek::fromCurrent(n)));
}

Base::Result<size_t> skip(Readable auto &reader, size_t n) {
    Sink sink;
    return copy(reader, sink, n);
}

/* --- Copy ----------------------------------------------------------------- */

Base::Result<size_t> copy(Readable auto &reader, Writable auto &writer) {
    uint8_t buffer[4096];
    size_t result = 0;
    while (true) {
        auto read = try$(reader.read(buffer, sizeof(buffer)));
        if (read == 0) {
            break;
        }
        result += read;
        auto written = try$(writer.write(buffer, read));
        if (written != read) {
            return result;
        }
    }
    return result;
}

Base::Result<size_t> copy(Readable auto &reader, Writable auto &writer, size_t size) {
    uint8_t buffer[4096];
    size_t result = 0;
    while (size > 0) {
        auto read = try$(reader.read(buffer, Base::min(size, sizeof(buffer))));
        if (read == 0) {
            break;
        }
        result += read;
        auto written = try$(writer.write(buffer, read));
        if (written != read) {
            return result;
        }
        size -= read;
    }
    return result;
}

} // namespace Karm::Io
