#pragma once

#include "file.h"

namespace Karm::Sys {

struct Pipe {
    FileWriter in;
    FileReader out;

    static Res<Pipe> create();
};

} // namespace Karm::Sys
