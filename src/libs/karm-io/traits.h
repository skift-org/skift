#pragma once

#include <karm-base/rune.h>
#include <karm-base/string.h>
#include <karm-sys/defs.h>

#include "types.h"

namespace Karm::Io {

template <typename T>
concept Writable = requires(T& writer, Bytes bytes) {
    { writer.write(bytes) } -> Meta::Same<Res<usize>>;
};

template <typename T>
concept Readable = requires(T& reader, MutBytes bytes) {
    { reader.read(bytes) } -> Meta::Same<Res<usize>>;
};

template <typename T>
concept Seekable = requires(T& seeker, Seek seek) {
    { seeker.seek(seek) } -> Meta::Same<Res<usize>>;
};

template <typename T>
concept Flushable = requires(T& flusher) {
    { flusher.flush() } -> Meta::Same<Res<>>;
};

template <typename T>
concept Duplexable = Writable<T> and Readable<T>;

template <typename T>
concept SeekableWritable = Writable<T> and Seekable<T>;

template <typename T>
concept SeekableReadable = Readable<T> and Seekable<T>;

template <typename T>
concept SeekableDuplexable = Duplexable<T> and Seekable<T>;

struct Writer {
    virtual ~Writer() = default;

    virtual Res<usize> write(Bytes) = 0;
};

static_assert(Writable<Writer>);

struct Reader {
    virtual ~Reader() = default;

    virtual Res<usize> read(MutBytes) = 0;
};

static_assert(Readable<Reader>);

struct Seeker {
    virtual ~Seeker() = default;

    virtual Res<usize> seek(Seek seek) = 0;
};

static_assert(Seekable<Seeker>);

struct Flusher {
    virtual ~Flusher() = default;

    virtual Res<> flush() = 0;
};

static_assert(Flushable<Flusher>);

} // namespace Karm::Io
