#pragma once

#include <karm-ui/node.h>

namespace Demos {

struct Demo {
    Mdi::Icon icon;
    Str name;
    Str description;
    Func<Ui::Child()> build;
};

} // namespace Demos
