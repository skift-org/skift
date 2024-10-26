#pragma once

#include <karm-io/traits.h>
#include <karm-meta/traits.h>

namespace Karm {

Res<usize> pack(Io::Writable auto &writer, Meta::Trivial auto const &v) {
    return writer.write(&v, sizeof(v));
}

Res<usize> pack(Io::Writable auto &writer, Meta::Trivial auto... v) {
    return (pack(writer, v) + ...);
}

Res<usize> unpack(Io::Readable auto &reader, Meta::Trivial auto &v) {
    return reader.read(&v, sizeof(v));
}

Res<usize> unpack(Io::Readable auto &reader, Meta::Trivial auto... v) {
    return (unpack(reader, v) + ...);
}

} // namespace Karm
