#pragma once

#include <karm-math/fixed.h>
#include <karm-math/insets.h>
#include <karm-math/radii.h>
#include <karm-math/rect.h>

// NOTE: Not in the Karm::Math namespace because it's pretty commonly used
//       and typing Math::Au is a bit too much.
namespace Karm {

// Au (aka Application Unit, Atomic Unit, or Awesome Unit) is the fundamental
// unit of measurement in Karm.
// It's inspired by Mozilla's AppUnits, see:
//  - https://docs.rs/app_units/latest/app_units/
//  - https://bugzilla.mozilla.org/show_bug.cgi?id=177805
using Au = Math::i24f8;

using RectAu = Math::Rect<Au>;

using Vec2Au = Math::Vec2<Au>;

using InsetsAu = Math::Insets<Au>;

using RadiiAu = Math::Radii<Au>;

} // namespace Karm

constexpr Karm::Au operator""_au(unsigned long long val) {
    return Karm::Au{val};
}
