#pragma once

#include <karm-ui/node.h>

namespace Hideo::Demos {

struct Demo {
    Mdi::Icon icon;
    Str name;
    Str description;
    Func<Ui::Child()> build;
};

struct Edit {
    usize index;
    Math::Vec2f pos;
};

} // namespace Hideo::Demos
