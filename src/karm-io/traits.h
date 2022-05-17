#pragma once

#include "types.h"

namespace Karm::Io {

template <typename T>
concept Writable = requires(T &writer, void const *data, size_t size) {
    { writer.write(data, size) } -> Meta::Same<Base::Result<size_t>>;
};

template <typename T>
concept Readable = requires(T &reader, void *data, size_t size) {
    { reader.read(data, size) } -> Meta::Same<Base::Result<size_t>>;
};

template <typename T>
concept Seekable = requires(T &seeker, Seek seek) {
    { seeker.seek(seek) } -> Meta::Same<Base::Result<size_t>>;
};

template <typename T>
concept Flushable = requires(T &flusher) {
    { flusher.flush() } -> Meta::Same<Base::Result<size_t>>;
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
    virtual Base::Result<size_t> write(void const *data, size_t size) = 0;
};

static_assert(Writable<Writer>);

struct Reader {
    virtual ~Reader() = default;
    virtual Base::Result<size_t> read(void *data, size_t size) = 0;
};

static_assert(Readable<Reader>);

struct Seeker {
    virtual ~Seeker() = default;
    virtual Base::Result<size_t> seek(Seek seek) = 0;
};

static_assert(Seekable<Seeker>);

struct Flusher {
    virtual ~Flusher() = default;
    virtual Base::Result<size_t> flush() = 0;
};

static_assert(Flushable<Flusher>);

} // namespace Karm::Io
