#pragma once

#include <karm-sys/context.h>

namespace Karm::App {

enum struct FormFactor {
    DESKTOP,
    MOBILE,
};

static constexpr FormFactor formFactor = FormFactor::DESKTOP;

} // namespace Karm::App
