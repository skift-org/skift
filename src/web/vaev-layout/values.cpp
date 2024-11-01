#include "values.h"

#include "box.h"
#include "writing.h"

namespace Vaev::Layout {

Resolver Resolver::from(Tree const &tree, Box const &box) {
    Resolver resolver;
    resolver.rootFont = Text::Font{tree.root.fontFace, tree.root.layout.fontSize.cast<f64>()};
    resolver.boxFont = Text::Font{box.fontFace, box.layout.fontSize.cast<f64>()};
    resolver.viewport = tree.viewport;
    resolver.boxAxis = mainAxis(box);
    return resolver;
}

Resolver Resolver::inherit(Resolver const &resolver) {
    Resolver copy = resolver;
    copy.boxFont = NONE;
    copy.parentFontSize = resolver.boxFont->fontsize;
    return copy;
}

Px Resolver::_resolveFontRelative(Length value) {
    if (not boxFont or not rootFont) {
        logWarn("missing font information");
        return Px{0};
    }

    switch (value.unit()) {

    case Length::EM:
        return Px::fromFloatNearest(value.val() * boxFont->fontSize());

    case Length::REM:
        return Px::fromFloatNearest(value.val() * rootFont->fontSize());

    case Length::EX:
        return Px::fromFloatNearest(value.val() * boxFont->xHeight());

    case Length::REX:
        return Px::fromFloatNearest(value.val() * rootFont->xHeight());

    case Length::CAP:
        return Px::fromFloatNearest(value.val() * boxFont->capHeight());

    case Length::RCAP:
        return Px::fromFloatNearest(value.val() * rootFont->capHeight());

    case Length::CH:
        return Px::fromFloatNearest(value.val() * boxFont->zeroAdvance());

    case Length::RCH:
        return Px::fromFloatNearest(value.val() * rootFont->zeroAdvance());

    case Length::IC:
        return Px::fromFloatNearest(value.val() * boxFont->zeroAdvance());

    case Length::RIC:
        return Px::fromFloatNearest(value.val() * rootFont->zeroAdvance());

    case Length::LH:
        return Px::fromFloatNearest(value.val() * boxFont->lineHeight());

    case Length::RLH:
        return Px::fromFloatNearest(value.val() * rootFont->lineHeight());

    default:
        panic("expected font-relative unit");
    }
}

Px Resolver::resolve(Length value) {
    if (value.isFontRelative())
        return _resolveFontRelative(value);
    switch (value.unit()) {

    // Viewport-relative

    // https://drafts.csswg.org/css-values/#vw

    // Equal to 1% of the width of current viewport.
    case Length::VW:
    case Length::LVW:
        return Px::fromFloatNearest(value.val() * viewport.large.width.cast<f64>() / 100);

    case Length::SVW:
        return Px::fromFloatNearest(value.val() * viewport.small.width.cast<f64>() / 100);

    case Length::DVW:
        return Px::fromFloatNearest(value.val() * viewport.dynamic.width.cast<f64>() / 100);

    // https://drafts.csswg.org/css-values/#vh
    // Equal to 1% of the height of current viewport.
    case Length::VH:
    case Length::LVH:
        return Px::fromFloatNearest(value.val() * viewport.large.height.cast<f64>() / 100);

    case Length::SVH:
        return Px::fromFloatNearest(value.val() * viewport.small.height.cast<f64>() / 100);

    case Length::DVH:
        return Px::fromFloatNearest(value.val() * viewport.dynamic.height.cast<f64>() / 100);

    // https://drafts.csswg.org/css-values/#vi
    // Equal to 1% of the size of the viewport in the box’s inline axis.
    case Length::VI:
    case Length::LVI:
        if (boxAxis == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * viewport.large.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * viewport.large.height.cast<f64>() / 100);
        }

    case Length::SVI:
        if (boxAxis == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * viewport.small.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * viewport.small.height.cast<f64>() / 100);
        }

    case Length::DVI:
        if (boxAxis == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * viewport.dynamic.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * viewport.dynamic.height.cast<f64>() / 100);
        }

    // https://drafts.csswg.org/css-values/#vb
    // Equal to 1% of the size of the viewport in the box’s block axis.
    case Length::VB:
    case Length::LVB:
        if (boxAxis.cross() == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * viewport.large.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * viewport.large.height.cast<f64>() / 100);
        }

    case Length::SVB:
        if (boxAxis.cross() == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * viewport.small.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * viewport.small.height.cast<f64>() / 100);
        }

    case Length::DVB:
        if (boxAxis.cross() == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(value.val() * viewport.dynamic.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(value.val() * viewport.dynamic.height.cast<f64>() / 100);
        }

    // https://drafts.csswg.org/css-values/#vmin
    // Equal to the smaller of vw and vh.
    case Length::VMIN:
    case Length::LVMIN:
        return min(
            resolve(Length(value.val(), Length::VW)),
            resolve(Length(value.val(), Length::VH))
        );

    case Length::SVMIN:
        return min(
            resolve(Length(value.val(), Length::SVW)),
            resolve(Length(value.val(), Length::SVH))
        );

    case Length::DVMIN:
        return min(
            resolve(Length(value.val(), Length::DVW)),
            resolve(Length(value.val(), Length::DVH))
        );

    // https://drafts.csswg.org/css-values/#vmax
    // Equal to the larger of vw and vh.
    case Length::VMAX:
    case Length::LVMAX:
        return max(
            resolve(Length(value.val(), Length::VW)),
            resolve(Length(value.val(), Length::VH))
        );

    case Length::DVMAX:
        return max(
            resolve(Length(value.val(), Length::DVW)),
            resolve(Length(value.val(), Length::DVH))
        );

    case Length::SVMAX:
        return max(
            resolve(Length(value.val(), Length::SVW)),
            resolve(Length(value.val(), Length::SVH))
        );

    // Absolute
    // https://drafts.csswg.org/css-values/#absolute-lengths
    case Length::CM:
        return Px::fromFloatNearest(value.val() * viewport.dpi.cast<f64>() / 2.54);

    case Length::MM:
        return Px::fromFloatNearest(value.val() * viewport.dpi.cast<f64>() / 25.4);

    case Length::Q:
        return Px::fromFloatNearest(value.val() * viewport.dpi.cast<f64>() / 101.6);

    case Length::IN:
        return Px::fromFloatNearest(value.val() * viewport.dpi.cast<f64>());

    case Length::PT:
        return Px::fromFloatNearest(value.val() * viewport.dpi.cast<f64>() / 72.0);

    case Length::PC:
        return Px::fromFloatNearest(value.val() * viewport.dpi.cast<f64>() / 6.0);

    case Length::PX:
        return Px::fromFloatNearest(value.val());

    default:
        panic("invalid length unit");
    }
}

Px Resolver::resolve(PercentOr<Length> value, Px relative) {
    if (value.resolved())
        return resolve(value.value());
    return Px{relative.cast<f64>() * (value.percent().value() / 100.)};
}

Px Resolver::resolve(Width value, Px relative) {
    if (value == Width::Type::AUTO)
        return Px{0};
    return resolve(value.value, relative);
}

Px Resolver::resolve(FontSize value) {
    switch (value.named()) {
    case FontSize::XX_SMALL:
        return Px::fromFloatNearest(userFontSize * 0.5);
    case FontSize::X_SMALL:
        return Px::fromFloatNearest(userFontSize * 0.75);
    case FontSize::SMALL:
        return Px::fromFloatNearest(userFontSize * 0.875);
    case FontSize::MEDIUM:
        return Px::fromFloatNearest(userFontSize);
    case FontSize::LARGE:
        return Px::fromFloatNearest(userFontSize * 1.125);
    case FontSize::X_LARGE:
        return Px::fromFloatNearest(userFontSize * 1.25);
    case FontSize::XX_LARGE:
        return Px::fromFloatNearest(userFontSize * 1.5);

    case FontSize::LARGER:
        return Px::fromFloatNearest(parentFontSize * 1.25);
    case FontSize::SMALLER:
        return Px::fromFloatNearest(parentFontSize * 0.875);

    case FontSize::LENGTH:
        return resolve(value.value(), Px{parentFontSize});

    default:
        panic("unimplemented font size");
        break;
    }
}

// MARK: Resolve during layout -------------------------------------------------

Px resolve(Tree const &tree, Box const &box, Length value) {
    return Resolver::from(tree, box).resolve(value);
}

Px resolve(Tree const &tree, Box const &box, PercentOr<Length> value, Px relative) {
    return Resolver::from(tree, box).resolve(value, relative);
}

Px resolve(Tree const &tree, Box const &box, Width value, Px relative) {
    return Resolver::from(tree, box).resolve(value, relative);
}

Px resolve(Tree const &tree, Box const &box, FontSize value) {
    return Resolver::from(tree, box).resolve(value);
}

} // namespace Vaev::Layout
