#pragma once

#include <karm-base/rc.h>
#include <karm-io/types.h>
#include <karm-meta/same.h>

#include "path.h"

namespace Karm::Sys {

struct Fd {
    virtual ~Fd() = default;

    virtual Base::Result<size_t> read(void *buf, size_t size) = 0;

    virtual Base::Result<size_t> write(void const *buf, size_t size) = 0;

    virtual Base::Result<size_t> seek(Io::Seek seek) = 0;

    virtual Base::Result<size_t> flush() = 0;

    virtual Base::Result<Base::Strong<Fd>> dup() = 0;
};

template <typename T>
concept AsFd = requires(T t) {
    { t.fd() } -> Meta::Same<Base::Strong<Fd>>;
};

} // namespace Karm::Sys
