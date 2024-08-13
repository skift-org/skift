#include "context.h"

#include "frag.h"

namespace Vaev::Layout {

// MARK: Child/Parent relationship ---------------------------------------

Context const &Context::root() const {
    if (&parent == this)
        return *this;
    return parent.root();
}

Context Context::subContext(Frag &frag, Axis axis, RectPx containingBlock) const {
    return Context{
        viewport,
        *this,
        frag,
        axis,
        containingBlock,
        font,
    };
}

// MARK: Fragment ----------------------------------------------------------

Style::Computed const &Context::style() {
    return *frag.style;
}

MutSlice<Frag> Context::children() {
    return frag.children();
}

// MARK: Font --------------------------------------------------------------

Px Context::fontSize() {
    return Px{font.fontsize};
}

Px Context::xHeight() {
    return Px{font.measure(font.glyph('x')).capbound.height};
}

Px Context::capHeight() {
    return Px{font.measure(font.glyph('H')).capbound.height};
}

Px Context::zeroAdvance() {
    return Px{font.advance(font.glyph('0'))};
}

Px Context::lineHeight() {
    return Px(font.metrics().lineheight());
}

// MARK: Values ------------------------------------------------------------

Px Context::resolve(Length l) {
    switch (l.unit()) {
        // Font-relative

    case Length::Unit::EM:
        return Px::fromFloatNearest(l.val() * fontSize().cast<f64>());

    case Length::Unit::REM:
        return Px::fromFloatNearest(l.val() * fontSize().cast<f64>());

    case Length::Unit::EX:
        return Px::fromFloatNearest(l.val() * xHeight().cast<f64>());

    case Length::Unit::REX:
        return Px::fromFloatNearest(l.val() * xHeight().cast<f64>());

    case Length::Unit::CAP:
        return Px::fromFloatNearest(l.val() * capHeight().cast<f64>());

    case Length::Unit::RCAP:
        return Px::fromFloatNearest(l.val() * capHeight().cast<f64>());

    case Length::Unit::CH:
        return Px::fromFloatNearest(l.val() * zeroAdvance().cast<f64>());

    case Length::Unit::RCH:
        return Px::fromFloatNearest(l.val() * zeroAdvance().cast<f64>());

    case Length::Unit::IC:
        return Px::fromFloatNearest(l.val() * zeroAdvance().cast<f64>());

    case Length::Unit::RIC:
        return Px::fromFloatNearest(l.val() * zeroAdvance().cast<f64>());

    case Length::Unit::LH:
        return Px::fromFloatNearest(l.val() * lineHeight().cast<f64>());

    case Length::Unit::RLH:
        return Px::fromFloatNearest(l.val() * lineHeight().cast<f64>());

    // Viewport-relative

    // https://drafts.csswg.org/css-values/#vw

    // Equal to 1% of the width of current viewport.
    case Length::Unit::VW:
    case Length::Unit::LVW:
        return Px::fromFloatNearest(l.val() * viewport.large.width.cast<f64>() / 100);

    case Length::Unit::SVW:
        return Px::fromFloatNearest(l.val() * viewport.small.width.cast<f64>() / 100);

    case Length::Unit::DVW:
        return Px::fromFloatNearest(l.val() * viewport.dynamic.width.cast<f64>() / 100);

    // https://drafts.csswg.org/css-values/#vh
    // Equal to 1% of the height of current viewport.
    case Length::Unit::VH:
    case Length::Unit::LVH:
        return Px::fromFloatNearest(l.val() * viewport.large.height.cast<f64>() / 100);

    case Length::Unit::SVH:
        return Px::fromFloatNearest(l.val() * viewport.small.height.cast<f64>() / 100);

    case Length::Unit::DVH:
        return Px::fromFloatNearest(l.val() * viewport.dynamic.height.cast<f64>() / 100);

    // https://drafts.csswg.org/css-values/#vi
    // Equal to 1% of the size of the viewport in the box’s inline axis.
    case Length::Unit::VI:
    case Length::Unit::LVI:
        if (mainAxis() == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * viewport.large.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * viewport.large.height.cast<f64>() / 100);
        }

    case Length::Unit::SVI:
        if (mainAxis() == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * viewport.small.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * viewport.small.height.cast<f64>() / 100);
        }

    case Length::Unit::DVI:
        if (mainAxis() == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * viewport.dynamic.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * viewport.dynamic.height.cast<f64>() / 100);
        }

    // https://drafts.csswg.org/css-values/#vb
    // Equal to 1% of the size of the viewport in the box’s block axis.
    case Length::Unit::VB:
    case Length::Unit::LVB:
        if (crossAxis() == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * viewport.large.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * viewport.large.height.cast<f64>() / 100);
        }

    case Length::Unit::SVB:
        if (crossAxis() == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * viewport.small.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * viewport.small.height.cast<f64>() / 100);
        }

    case Length::Unit::DVB:
        if (crossAxis() == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * viewport.dynamic.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * viewport.dynamic.height.cast<f64>() / 100);
        }

    // https://drafts.csswg.org/css-values/#vmin
    // Equal to the smaller of vw and vh.
    case Length::Unit::VMIN:
    case Length::Unit::LVMIN:
        return min(
            resolve(Length(l.val(), Length::Unit::VW)),
            resolve(Length(l.val(), Length::Unit::VH))
        );

    case Length::Unit::SVMIN:
        return min(
            resolve(Length(l.val(), Length::Unit::SVW)),
            resolve(Length(l.val(), Length::Unit::SVH))
        );

    case Length::Unit::DVMIN:
        return min(
            resolve(Length(l.val(), Length::Unit::DVW)),
            resolve(Length(l.val(), Length::Unit::DVH))
        );

    // https://drafts.csswg.org/css-values/#vmax
    // Equal to the larger of vw and vh.
    case Length::Unit::VMAX:
    case Length::Unit::LVMAX:
        return max(
            resolve(Length(l.val(), Length::Unit::VW)),
            resolve(Length(l.val(), Length::Unit::VH))
        );

    case Length::Unit::DVMAX:
        return max(
            resolve(Length(l.val(), Length::Unit::DVW)),
            resolve(Length(l.val(), Length::Unit::DVH))
        );

    case Length::Unit::SVMAX:
        return max(
            resolve(Length(l.val(), Length::Unit::SVW)),
            resolve(Length(l.val(), Length::Unit::SVH))
        );

    // Absolute
    // https://drafts.csswg.org/css-values/#absolute-lengths
    case Length::Unit::CM:
        return Px::fromFloatNearest(l.val() * viewport.dpi.cast<f64>() / 2.54);

    case Length::Unit::MM:
        return Px::fromFloatNearest(l.val() * viewport.dpi.cast<f64>() / 25.4);

    case Length::Unit::Q:
        return Px::fromFloatNearest(l.val() * viewport.dpi.cast<f64>() / 101.6);

    case Length::Unit::IN:
        return Px::fromFloatNearest(l.val() * viewport.dpi.cast<f64>());

    case Length::Unit::PT:
        return Px::fromFloatNearest(l.val() * viewport.dpi.cast<f64>() / 72.0);

    case Length::Unit::PC:
        return Px::fromFloatNearest(l.val() * viewport.dpi.cast<f64>() / 6.0);

    case Length::Unit::PX:
        return Px::fromFloatNearest(l.val());

    default:
        panic("invalid length unit");
    }
}

Px Context::resolve(PercentOr<Length> p, Px relative) {
    if (p.resolved())
        return resolve(p.value());
    return Px{relative.cast<f64>() * (p.percent().value() / 100.)};
}

Px Context::resolve(Width w, Px relative) {
    if (w == Width::Type::AUTO)
        return Px{0};
    return resolve(w.value, relative);
}

} // namespace Vaev::Layout
