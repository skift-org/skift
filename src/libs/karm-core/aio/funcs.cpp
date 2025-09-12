module;

#include <karm-core/macros.h>

export module Karm.Core:aio.funcs;

import :io.text;
import :aio.traits;

namespace Karm::Aio {

export Async::Task<usize> copyAsync(AsyncReadable auto& reader, AsyncWritable auto& writer) {
    Array<u8, 4096> buffer = {};
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

export Async::Task<String> readAllUtf8Async(AsyncReadable auto& reader) {
    Io::StringWriter w;
    Array<Utf8::Unit, 512> buf = {};
    while (true) {
        usize read = co_trya$(reader.readAsync(buf.mutBytes()));
        if (read == 0)
            break;
        co_try$(w.writeUnit(sub(buf, 0, read)));
    }
    co_return Ok(w.take());
}

} // namespace Karm::Aio
