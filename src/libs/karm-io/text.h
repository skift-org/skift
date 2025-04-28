#pragma once

#include "traits.h"

namespace Karm::Io {

struct TextWriter : Writer, Flusher {
    using Writer::write;

    template <StaticEncoding E>
    Res<> writeStr(_Str<E> str) {
        for (auto rune : iterRunes(str))
            try$(writeRune(rune));
        return Ok();
    }

    virtual Res<> writeRune(Rune rune) = 0;

    Res<> flush() override {
        return Ok();
    }
};

template <StaticEncoding E = typename Sys::Encoding>
struct TextEncoderBase : TextWriter {

    using Writer::write;

    Res<> writeRune(Rune rune) override {
        typename E::One one;
        if (not E::encodeUnit(rune, one))
            return Error::invalidInput("encoding error");
        try$(write(bytes(one)));
        return Ok();
    }
};

template <StaticEncoding E = typename Sys::Encoding>
struct TextEncoder : TextEncoderBase<E> {

    Io::Writer& _writer;

    TextEncoder(Io::Writer& writer)
        : _writer(writer) {}

    Res<usize> write(Bytes bytes) override {
        return _writer.write(bytes);
    }
};

template <StaticEncoding E>
struct _StringWriter : TextWriter, _StringBuilder<E> {

    _StringWriter(usize cap = 16) : _StringBuilder<E>(cap) {}

    Res<usize> write(Bytes) override {
        panic("can't write raw bytes to a string");
    }

    Res<> writeRune(Rune rune) override {
        _StringBuilder<E>::append(rune);
        return Ok();
    }

    Res<> writeUnit(Slice<typename E::Unit> unit) {
        _StringBuilder<E>::append(unit);
        return Ok();
    }
};

using StringWriter = _StringWriter<Utf8>;

} // namespace Karm::Io
