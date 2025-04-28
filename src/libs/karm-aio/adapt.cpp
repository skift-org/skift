module;

#include <karm-async/task.h>
#include <karm-io/traits.h>

export module Karm.Aio:adapt;

import :traits;

namespace Karm::Aio {

export template <typename T>
struct Adapter : Reader, Writer {
    T& _inner;

    Adapter(T& inner)
        : _inner(inner) {}

    Async::Task<usize> writeAsync(Bytes buf) override {
        if constexpr (Io::Writable<T>) {
            co_return _inner.write(buf);
        } else {
            co_return Ok(buf.len());
        }
    }

    Async::Task<usize> readAsync(MutBytes buf) override {
        if constexpr (Io::Readable<T>) {
            co_return _inner.read(buf);
        } else {
            co_return Ok(0);
        }
    }
};

export template <typename T>
Adapter<T> adapt(T& t) {
    return {t};
}

} // namespace Karm::Aio
