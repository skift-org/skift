#pragma once

#include <karm-base/slice.h>
#include <karm-base/std.h>
#include <karm-io/funcs.h>
#include <karm-io/impls.h>
#include <karm-io/traits.h>

namespace Adler {

struct Adler32 {
    u32 sum;

    void add(Bytes bytes) {
        u32 s1 = sum & 0xffff;
        u32 s2 = sum >> 16;

        for (usize i = 0; i < bytes.len(); ++i) {
            s1 = (s1 + bytes[i]) % 65521;
            s2 = (s2 + s1) % 65521;
        }

        sum = (s2 << 16) + s1;
    }
};

struct Adler32Reader : public Io::Reader {
    Adler32 adler;
    Io::Reader &reader;

    Adler32Reader(Io::Reader &reader)
        : reader(reader) {}

    Res<usize> read(MutBytes bytes) override {
        auto len = try$(reader.read(bytes));
        adler.add(sub(bytes, 0, len));
        return Ok(len);
    }

    u32 sum() const {
        return adler.sum;
    }
};

struct Adler32Writer : public Io::Writer {
    Adler32 adler;
    Io::Writer &writer;

    Adler32Writer(Io::Writer &writer)
        : writer(writer) {}

    Res<usize> write(Bytes bytes) override {
        adler.add(bytes);
        return writer.write(bytes);
    }

    u32 sum() const {
        return adler.sum;
    }
};

static inline u32 adler32(Bytes bytes) {
    Adler32 adler;
    adler.add(bytes);
    return adler.sum;
}

static inline Res<u32> adler32(Io::Reader &reader) {
    Adler32Reader adlerReader(reader);
    Io::Sink sink{};
    try$(Io::copy(adlerReader, sink));
    return Ok(adlerReader.sum());
}

} // namespace Adler
