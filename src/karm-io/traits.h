#pragma once

#include "types.h"

namespace Karm::Io {

template <typename T>
concept Writer = requires(T const &writer, void const *data, size_t size) {
    { writer.write(data, size) } -> Meta::Same<Base::Result<size_t>>;
};

template <typename T>
concept Reader = requires(T const &reader, void *data, size_t size) {
    { reader.read(data, size) } -> Meta::Same<Base::Result<size_t>>;
};

template <typename T>
concept Seeker = requires(T const &seeker, Seek seek) {
    { seeker.seek(seek) } -> Meta::Same<Base::Result<size_t>>;
};

template <typename T>
concept Duplex = Writer<T> && Reader<T>;

template <typename T>
concept SeekableWriter = Writer<T> && Seeker<T>;

template <typename T>
concept SeekableReader = Reader<T> && Seeker<T>;

template <typename T>
concept SeekableDuplex = Duplex<T> && Seeker<T>;

template <typename T>
concept Flusher = requires(T const &flusher) {
    { flusher.flush() } -> Meta::Same<Base::Result<size_t>>;
};

} // namespace Karm::Io
