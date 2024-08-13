#pragma once

#include <karm-text/font.h>
#include <vaev-base/width.h>
#include <vaev-base/writing.h>
#include <vaev-style/computed.h>

#include "base.h"

namespace Vaev::Layout {

struct Frag;

struct Context {
    Viewport const &viewport;
    Context const &parent = *this;
    Frag &frag;

    Axis axis;
    RectPx containingBlock;
    Text::Font font;

    // MARK: Child/Parent relationship -----------------------------------------

    Context const &root() const;

    Context subContext(Frag &frag, Axis axis, RectPx containingBlock) const;

    // MARK: Fragment ----------------------------------------------------------

    Style::Computed const &style();

    MutSlice<Frag> children();

    // MARK: Axis --------------------------------------------------------------

    Axis mainAxis() const {
        return axis;
    }

    Axis crossAxis() const {
        return axis == Axis::VERTICAL ? Axis::HORIZONTAL : Axis::VERTICAL;
    }

    // MARK: Font --------------------------------------------------------------

    Px fontSize();

    Px xHeight();

    Px capHeight();

    Px zeroAdvance();

    Px lineHeight();

    // MARK: Values ------------------------------------------------------------

    Px resolve(Length l);

    Px resolve(PercentOr<Length> l, Px relative);

    Px resolve(Width w, Px relative);
};

} // namespace Vaev::Layout
