#pragma once

#include <web-base/background.h>
#include <web-base/borders.h>

#include "base.h"

namespace Web::Paint {

struct Box : public Node {
    Math::Recti bound;
    Borders borders;
    Vec<Background> backgrounds;
};

} // namespace Web::Paint
