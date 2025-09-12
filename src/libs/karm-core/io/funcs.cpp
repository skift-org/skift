module;

#include <karm-core/macros.h>

export module Karm.Core:io.funcs;

import :base.ring;
import :io.impls;
import :io.text;

namespace Karm::Io {

// MARK: Read ------------------------------------------------------------------

export Res<usize> pread(Readable auto& reader, MutBytes bytes, Seek seek) {
    auto result = try$(reader.seek(seek));
    return reader.read(bytes);
}

export Res<u8> getByte(Readable auto& reader) {
    u8 byte;
    try$(reader.read({&byte, 1}));
    return Ok(byte);
}

export Res<String> readAllUtf8(Readable auto& reader) {
    StringWriter writer;
    Array<Utf8::Unit, 512> buf;
    while (true) {
        usize read = try$(reader.read(buf.mutBytes()));
        if (read == 0) {
            break;
        }
        try$(writer.writeUnit({buf.buf(), read}));
    }
    return Ok(writer.take());
}

// MARK: Write -----------------------------------------------------------------

export Res<usize> pwrite(Writable auto& writer, Bytes bytes, Seek seek) {
    auto result = try$(writer.seek(seek));
    return writer.write(bytes);
}

export Res<usize> putByte(Writable auto& writer, u8 byte) {
    return writer.write({&byte, 1});
}

// MARK: Seek ------------------------------------------------------------------

export Res<usize> tell(Seekable auto& seeker) {
    return seeker.seek(Seek::fromCurrent(0));
}

export Res<usize> size(Seekable auto& seeker) {
    usize current = try$(tell(seeker));
    usize end = try$(seeker.seek(Seek::fromEnd(0)));
    try$(seeker.seek(Seek::fromBegin(current)));
    return Ok(end);
}

export Res<usize> skip(Seekable auto& seeker, usize n) {
    return seeker.seek(Seek::fromCurrent(n));
}

export Res<usize> skip(Readable auto& reader, usize n) {
    Sink sink;
    return copy(reader, sink, n);
}

// MARK: Copy ------------------------------------------------------------------

export Res<usize> copy(Readable auto& reader, MutBytes bytes) {
    usize readed = 0;
    while (readed < bytes.len()) {
        readed += try$(reader.read(next(bytes, readed)));
    }
    return Ok(readed);
}

export Res<usize> copy(Readable auto& reader, Writable auto& writer) {
    Array<u8, 4096> buffer;
    usize result = 0;
    while (true) {
        auto read = try$(reader.read(mutBytes(buffer)));
        if (read == 0)
            return Ok(result);

        result += read;

        auto written = try$(writer.write(sub(buffer, 0, read)));
        if (written != read)
            return Ok(result);
    }
}

export Res<usize> copy(Readable auto& reader, Writable auto& writer, usize size) {
    Array<u8, 4096> buf;
    usize result = 0;
    while (size > 0) {
        auto read = try$(reader.read(mutSub(buf, 0, size)));
        if (read == 0)
            return Ok(result);

        result += read;

        auto written = try$(writer.write(sub(buf, 0, read)));
        if (written != read)
            return Ok(result);

        size -= read;
    }
    return Ok(result);
}

export Res<Tuple<usize, bool>> readLine(Readable auto& reader, Writable auto& writer, Bytes delim) {
    if (delim.len() > 16)
        panic("delimiter string too large");

    u8 b;
    usize result = 0;
    Ring<u8> lastBytes{delim.len()};

    while (true) {
        auto read = try$(reader.read({&b, 1}));

        if (read == 0)
            return Ok(Tuple<usize, bool>{result, false});

        result += read;

        if (lastBytes.rem() == 0)
            lastBytes.popFront();
        lastBytes.pushBack(b);

        auto written = try$(writer.write({&b, 1}));
        if (written != read)
            return Error::writeZero();

        if (lastBytes == delim) {
            result -= delim.len();
            break;
        }
    }
    return Ok(Tuple<usize, bool>{result, true});
}

} // namespace Karm::Io
