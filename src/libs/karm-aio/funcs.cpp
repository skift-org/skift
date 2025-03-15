module;

#include <karm-async/task.h>
#include <karm-base/array.h>

export module Karm.Aio:funcs;

import :traits;

namespace Karm::Aio {

export Async::Task<usize> copyAsync(AsyncReadable auto& reader, AsyncWritable auto& writer) {
    Array<Byte, 4096> buffer;
    usize result = 0;
    while (true) {
        auto read = co_trya$(reader.readAsync(mutBytes(buffer)));
        if (read == 0)
            co_return Ok(result);

        result += read;

        auto written = co_trya$(writer.writeAsync(sub(buffer, 0, read)));
        if (written != read)
            co_return Ok(result);
    }
}

} // namespace Karm::Aio
