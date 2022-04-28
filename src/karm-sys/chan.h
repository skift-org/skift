#pragma once

#include "fd.h"

namespace Karm::Sys {

struct In {
    Fd _fd{0};
};

struct Out {
    Fd _fd{1};
};

struct Err {
    Fd _fd{2};
};

In &in();

Out &out();

Err &err();

} // namespace Karm::Sys
