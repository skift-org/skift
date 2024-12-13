#pragma once

#include "box.h"
#include "fragmentainer.h"

namespace Vaev::Layout {
struct Tree {
    Box root;
    Viewport viewport = {};
    FragmentationContext fc = {};
};
} // namespace Vaev::Layout
