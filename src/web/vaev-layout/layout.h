#pragma once

#include "base.h"

namespace Vaev::Layout {

InsetsAu computeMargins(Tree& tree, Box& box, Input input);

InsetsAu computeBorders(Tree& tree, Box& box);

Vec2Au computeIntrinsicSize(Tree& tree, Box& box, IntrinsicSize intrinsic, Vec2Au containingBlock);

Output layout(Tree& tree, Box& box, Input input);

Output layout(Tree& tree, Input input);

Tuple<Output, Frag> layoutCreateFragment(Tree& tree, Input input);

} // namespace Vaev::Layout
