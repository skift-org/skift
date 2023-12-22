#pragma once

#include <karm-base/array.h>
#include <karm-io/funcs.h>

namespace Karm::Crypto {

template <typename T>
concept HashFunc =
    requires(T t, Bytes bytes) {
        { t.add(bytes) } -> Meta::Same<void>;
        { t.reset() };
        { t.sum() };
        { t.digest() };
        typename T::Digest;
    };

template <typename T>
using HashDigest = typename T::Digest;

template <typename T>
using HashSum = decltype(Meta::declval<T>().sum());

template <usize bits, typename TAG>
struct Digest : public Array<u8, alignUp(bits, 8) / 8> {
};
/* --- Utilities ------------------------------------------------------------ */

template <HashFunc H>
struct SummerReader : public Io::Reader {
    Io::Reader &_reader;
    H _h;

    SummerReader(Io::Reader &reader, H h = {})
        : _reader(reader), _h(h) {}

    Res<usize> read(MutBytes bytes) override {
        auto len = try$(_reader.read(bytes));
        _h.add(sub(bytes, 0, len));
        return Ok(len);
    }

    HashSum<H> sum() const {
        return _h.sum();
    }

    HashDigest<H> digest() {
        return _h.digest();
    }
};

template <HashFunc H>
struct SummerWriter : public Io::Writer {
    Io::Writer &_writer;
    H _h;

    SummerWriter(Io::Writer &writer, H h = {})
        : _writer(writer), _h(h) {}

    Res<usize> write(Bytes bytes) override {
        auto len = try$(_writer.write(bytes));
        _h.add(sub(bytes, 0, len));
        return Ok(len);
    }

    HashSum<H> sum() const {
        return _h.sum;
    }

    HashDigest<H> digest() {
        return _h.digest();
    }
};

template <HashFunc H>
static inline HashSum<H> checksum(Bytes bytes, H h = {}) {
    h.add(bytes);
    return h.sum();
}

template <HashFunc H>
static inline Res<HashSum<H>> checksum(Io::Reader &reader, H h = {}) {
    SummerReader<H> s{reader, h};
    Io::Sink sink{};
    try$(Io::copy(s, sink));
    return Ok(s.sum());
}

template <HashFunc H>
static inline HashDigest<H> digest(Bytes bytes, H h = {}) {
    h.add(bytes);
    return h.digest();
}

template <HashFunc H>
static inline Res<HashDigest<H>> digest(Io::Reader &reader, H h = {}) {
    SummerReader<H> s{reader, h};
    Io::Sink sink{};
    try$(Io::copy(s, sink));
    return Ok(s.digest());
}

} // namespace Karm::Crypto
