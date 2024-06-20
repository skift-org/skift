#pragma once

#include <vaev-base/background.h>
#include <vaev-base/borders.h>

#include "base.h"

namespace Vaev::Paint {

struct Box : public Node {
    Math::Recti bound;
    Borders borders;
    Vec<Background> backgrounds;
};

} // namespace Vaev::Paint
