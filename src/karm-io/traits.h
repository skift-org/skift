#pragma once

#include <karm-base/rune.h>
#include <karm-base/string.h>

#include "types.h"

namespace Karm::Io {

template <typename T>
concept Writable = requires(T &writer, Bytes bytes) {
    { writer.write(bytes) } -> Meta::Same<Result<size_t>>;
};

template <typename T>
concept Readable = requires(T &reader, MutBytes bytes) {
    { reader.read(bytes) } -> Meta::Same<Result<size_t>>;
};

template <typename T>
concept Seekable = requires(T &seeker, Seek seek) {
    { seeker.seek(seek) } -> Meta::Same<Result<size_t>>;
};

template <typename T>
concept Flushable = requires(T &flusher) {
    { flusher.flush() } -> Meta::Same<Result<size_t>>;
};

template <typename T>
concept Duplexable = Writable<T> && Readable<T>;

template <typename T>
concept SeekableWritable = Writable<T> && Seekable<T>;

template <typename T>
concept SeekableReadable = Readable<T> && Seekable<T>;

template <typename T>
concept SeekableDuplexable = Duplexable<T> && Seekable<T>;

struct Writer {
    virtual ~Writer() = default;
    virtual Result<size_t> write(Bytes) = 0;
};

static_assert(Writable<Writer>);

struct Reader {
    virtual ~Reader() = default;
    virtual Result<size_t> read(MutBytes) = 0;
};

static_assert(Readable<Reader>);

struct Seeker {
    virtual ~Seeker() = default;
    virtual Result<size_t> seek(Seek seek) = 0;
};

static_assert(Seekable<Seeker>);

struct Flusher {
    virtual ~Flusher() = default;

    virtual Result<size_t> flush() = 0;
};

static_assert(Flushable<Flusher>);

struct _TextWriter : public Writer {
    using Writer::write;

    virtual Result<size_t> writeStr(Str str) = 0;
    virtual Result<size_t> writeRune(Rune rune) = 0;
};

template <StaticEncoding E>
struct TextWriter : public _TextWriter {
    using Writer::write;

    Result<size_t> writeStr(Str str) override {
        size_t written = 0;
        for (auto rune : str.runes()) {
            written += try$(writeRune(rune));
        }
        return written;
    }

    Result<size_t> writeRune(Rune rune) override {
        typename E::One one;
        if (!E::encodeUnit(rune, one)) {
            return 0;
        }
        return write(one.bytes());
    }
};

} // namespace Karm::Io
