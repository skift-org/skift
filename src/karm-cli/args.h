#pragma once

#include <karm-base/string.h>

#include "base.h"

namespace Karm::Cli {

struct Args {
    int _argc;
    char const **_argv;

    Str self() const {
        return _argv[0];
    }

    size_t len() const {
        return _argc - 1;
    }

    Str operator[](size_t i) const {
        return _argv[i + 1];
    }
};

struct Cli {
    Result eval(Args const &args);
};

} // namespace Karm::Cli
