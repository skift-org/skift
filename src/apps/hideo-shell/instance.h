#pragma once

#include <karm-gfx/icon.h>
#include <karm-ui/node.h>

namespace Hideo::Shell {

struct Instance;

struct Manifest {
    Mdi::Icon icon;
    String name;
    Gfx::ColorRamp ramp;
};

struct Instance {
    usize id;
    Math::Recti bound;
    Gfx::Color color;
    Strong<Manifest> manifest;
};

} // namespace Hideo::Shell
