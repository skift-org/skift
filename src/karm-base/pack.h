#pragma once

#include <karm-io/traits.h>
#include <karm-meta/traits.h>

#include "_prelude.h"

namespace Karm::Base {

auto pack(Io::Writer auto &writer, Meta::Trivial auto const &v) -> Result<size_t> {
    return writer.write(&v, sizeof(v));
}

auto pack(Io::Writer auto &writer, Meta::Trivial auto... v) -> Result<size_t> {
    return (pack(writer, v) + ...);
}

auto unpack(Io::Reader auto &reader, Meta::Trivial auto &v) -> Result<size_t> {
    return reader.read(&v, sizeof(v));
}

auto unpack(Io::Reader auto &reader, Meta::Trivial auto... v) -> Result<size_t> {
    return (unpack(reader, v) + ...);
}

} // namespace Karm::Base
