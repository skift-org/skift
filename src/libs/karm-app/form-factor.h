#pragma once

#include <karm-sys/context.h>

namespace Karm::App {

enum struct FormFactor {
    DESKTOP,
    MOBILE,
};

inline FormFactor useFormFactor(Sys::Context& ctx = Sys::globalContext()) {
    if (useArgs(ctx).has("--mobile"))
        return FormFactor::MOBILE;
    return FormFactor::DESKTOP;
}

} // namespace Karm::App
