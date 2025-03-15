module;

#include <karm-async/task.h>

export module Karm.Aio:traits;

namespace Karm::Aio {

template <typename T>
concept AsyncWritable = requires(T& writer, Bytes bytes) {
    { writer.writeAsync(bytes) } -> Meta::Same<Async::Task<usize>>;
};

template <typename T>
concept AsyncReadable = requires(T& reader, MutBytes bytes) {
    { reader.readAsync(bytes) } -> Meta::Same<Async::Task<usize>>;
};

export struct Writer {
    virtual ~Writer() = default;
    virtual Async::Task<usize> writeAsync(Bytes buf) = 0;
};

static_assert(AsyncWritable<Writer>);

export struct Reader {
    virtual ~Reader() = default;
    virtual Async::Task<usize> readAsync(MutBytes buf) = 0;
};

static_assert(AsyncReadable<Reader>);

} // namespace Karm::Aio
