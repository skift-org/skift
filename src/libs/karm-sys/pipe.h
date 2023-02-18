#pragma once

#include "fd.h"

namespace Karm::Sys {

struct Pipe {
    Strong<Fd> _in;
    Strong<Fd> _out;

    Strong<Fd> in() { return _in; }
    Strong<Fd> out() { return _out; }

    static Res<Pipe> create();
};

} // namespace Karm::Sys
