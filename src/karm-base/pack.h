#pragma once

#include <karm-io/types.h>
#include <karm-meta/traits.h>

namespace Karm::Base
{

template <Meta::Trivial T>
Io::Result pack(Io::Writer &writer, T const &v)
{
    return writer.write(&v, sizeof(v));
}

template <Meta::Trivial T>
Io::Result unpack(Io::Reader &reader, T &v)
{
    return reader.read(&v, sizeof(v));
}

} // namespace Karm::Base
