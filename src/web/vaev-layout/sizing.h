#pragma once

#include "base.h"
#include "context.h"
#include "frag.h"

namespace Vaev::Layout {

// MARK: Insets ----------------------------------------------------------------

Px computeInset(Context &ctx, Axis, Length inset);

Px computeInset(Context &ctx, Axis axis, PercentOr<Length> inset);

Px computeInset(Context &ctx, Axis axis, Width inset);

Px allInsets(Context &ctx, Axis axis);

// MARK: Content ---------------------------------------------------------------

// Compute the size of the content box from the specified size
Px computePreferredSize(Context &ctx, Axis axis, Px availableSpace = Px{});

// Compute the size of the content box from the specified size
Px computePreferredContentSize(Context &ctx, Axis axis, Px availableSpace = Px{});

// Compute the size of the border box from the specified size
Px computePreferredBorderSize(Context &ctx, Axis axis, Px availableSpace = Px{});

// Compute the size of the margin box from the specified size
Px computePreferredOuterSize(Context &ctx, Axis axis, Px availableSpace = Px{});

// MARK: Box -------------------------------------------------------------------

Box computeBox(Context &ctx, RectPx borderBox);

} // namespace Vaev::Layout
