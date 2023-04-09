#pragma once

#include <karm-base/string.h>

#include "base.h"

namespace Karm::Cli {

struct Args {
    isize _argc;
    char const **_argv;

    Str self() const {
        return _argv[0];
    }

    usize len() const {
        return _argc - 1;
    }

    Str operator[](usize i) const {
        if (i >= len()) {
            panic("out of range");
        }
        return _argv[i + 1];
    }

    bool has(Str arg) const {
        for (usize i = 0; i < len(); ++i) {
            if (Op::eq(this->operator[](i), arg)) {
                return true;
            }
        }
        return false;
    }
};

} // namespace Karm::Cli
