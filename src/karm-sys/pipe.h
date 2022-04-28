#pragma once

#include "fd.h"

namespace Karm::Sys {

struct Pipe {
    Fd _in;
    Fd _out;

    Fd &in() { return _in; }
    Fd &out() { return _out; }

    static Result<Pipe> create() {
        Embed::_Pipe pipe = try$(Embed::_pipe());
        return Pipe{Fd{pipe.in}, Fd{pipe.out}};
    }
};

} // namespace Karm::Sys
