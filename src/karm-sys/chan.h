#pragma once

#include <embed/sys.h>
#include <karm-io/traits.h>

#include "fd.h"

namespace Karm::Sys {

struct In : public Io::Reader {
    Base::Strong<Fd> _fd = Embed::createIn();
};

struct Out : public Io::Writer {
    Base::Strong<Fd> _fd = Embed::createOut();
};

struct Err : public Io::Writer {
    Base::Strong<Fd> _fd = Embed::createErr();
};

In &in();

Out &out();

Err &err();

} // namespace Karm::Sys
