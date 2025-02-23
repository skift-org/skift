module;

#include <karm-math/au.h>

export module Vaev.Layout:layout;

import :base;

namespace Vaev::Layout {

export InsetsAu computeMargins(Tree& tree, Box& box, Input input);

export InsetsAu computeBorders(Tree& tree, Box& box);

export Vec2Au computeIntrinsicSize(Tree& tree, Box& box, IntrinsicSize intrinsic, Vec2Au containingBlock);

export Output layout(Tree& tree, Box& box, Input input);

export Output layout(Tree& tree, Input input);

export Tuple<Output, Frag> layoutCreateFragment(Tree& tree, Input input);

} // namespace Vaev::Layout
