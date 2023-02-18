#pragma once

#include <karm-io/traits.h>
#include <karm-meta/traits.h>

#include "_prelude.h"

namespace Karm {

Res<size_t> pack(Io::Writable auto &writer, Meta::Trivial auto const &v) {
    return writer.write(&v, sizeof(v));
}

Res<size_t> pack(Io::Writable auto &writer, Meta::Trivial auto... v) {
    return (pack(writer, v) + ...);
}

Res<size_t> unpack(Io::Readable auto &reader, Meta::Trivial auto &v) {
    return reader.read(&v, sizeof(v));
}

Res<size_t> unpack(Io::Readable auto &reader, Meta::Trivial auto... v) {
    return (unpack(reader, v) + ...);
}

} // namespace Karm
