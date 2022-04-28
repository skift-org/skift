#pragma once

#include <karm-base/clamp.h>

#include "impls.h"
#include "traits.h"

namespace Karm::Io {

/* --- Read ----------------------------------------------------------------- */

auto pread(Reader auto &reader, void *data, size_t size, Seek seek) -> Base::Result<size_t> {
    auto result = try$(reader.seek(seek));
    return try$(reader.read(data, size));
}

auto getc(Reader auto &reader) -> Base::Result<uint8_t> {
    uint8_t byte;
    try$(reader.read(&byte, 1));
    return byte;
}

/* --- Write ---------------------------------------------------------------- */

auto pwrite(Writer auto &writer, void const *data, size_t size, Seek seek) -> Base::Result<size_t> {
    auto result = try$(writer.seek(seek));
    return try$(writer.write(data, size));
}

auto putc(Writer auto &writer, uint8_t byte) -> Base::Result<uint8_t> {
    try$(writer.write(&byte, 1));
    return byte;
}

/* --- Seek ----------------------------------------------------------------- */

auto tell(Seeker auto &seeker) -> Base::Result<size_t> {
    return try$(seeker.seek(Seek::from_current(0)));
}

auto size(Seeker auto &seeker) -> Base::Result<size_t> {
    size_t current = try$(tell(seeker));
    size_t end = try$(seeker.seek(Seek::from_end(0)));
    try$(seeker.seek(Seek::from_begin(current)));
    return end;
}

auto skip(Seeker auto &seeker, size_t n) -> Base::Result<size_t> {
    return try$(seeker.seek(Seek::from_current(n)));
}

auto skip(Reader auto &reader, size_t n) -> Base::Result<size_t> {
    Sink sink;
    return copy(reader, sink, n);
}

/* --- Copy ----------------------------------------------------------------- */

auto copy(Reader auto &reader, Writer auto &writer) -> Base::Result<size_t> {
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

auto copy(Reader auto &reader, Writer auto &writer, size_t size) -> Base::Result<size_t> {
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
