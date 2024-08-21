#pragma once

#include "base.h"
#include "frag.h"

namespace Vaev::Layout {

// MARK: Insets ----------------------------------------------------------------

Px computeInset(Tree &t, Frag &f, Axis, Length inset);

Px computeInset(Tree &t, Frag &f, Input input, Axis axis, PercentOr<Length> inset);

Px computeInset(Tree &t, Frag &f, Input input, Axis axis, Width inset);

Px allInsets(Tree &t, Frag &f, Input input, Axis axis);

// MARK: Content ---------------------------------------------------------------

// Compute the size of the content box from the specified size
Px computePreferredSize(Tree &t, Frag &f, Input input, Axis axis, Px availableSpace = Px{});

// Compute the size of the content box from the specified size
Px computePreferredContentSize(Tree &t, Frag &f, Input input, Axis axis, Px availableSpace = Px{});

// Compute the size of the border box from the specified size
Px computePreferredBorderSize(Tree &t, Frag &f, Input input, Axis axis, Px availableSpace = Px{});

// Compute the size of the margin box from the specified size
Px computePreferredOuterSize(Tree &t, Frag &f, Input input, Axis axis, Px availableSpace = Px{});

// MARK: Box -------------------------------------------------------------------

Box computeBox(Tree &t, Frag &f, Input input, RectPx borderBox);

} // namespace Vaev::Layout
