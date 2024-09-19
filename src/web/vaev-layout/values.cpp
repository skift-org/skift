#include "values.h"

#include "frag.h"
#include "writing.h"

namespace Vaev::Layout {

Px _resolveFontRelative(Tree &t, Frag &f, Length l) {
    Karm::Text::Font rootFont = {
        t.root.fontFace,
        t.root.layout.fontSize.cast<f64>(),
    };

    Karm::Text::Font fragFont = {
        f.fontFace,
        f.layout.fontSize.cast<f64>(),
    };

    switch (l.unit()) {

    case Length::Unit::EM:
        return Px::fromFloatNearest(l.val() * fragFont.fontSize());

    case Length::Unit::REM:
        return Px::fromFloatNearest(l.val() * rootFont.fontSize());

    case Length::Unit::EX:
        return Px::fromFloatNearest(l.val() * fragFont.xHeight());

    case Length::Unit::REX:
        return Px::fromFloatNearest(l.val() * rootFont.xHeight());

    case Length::Unit::CAP:
        return Px::fromFloatNearest(l.val() * fragFont.capHeight());

    case Length::Unit::RCAP:
        return Px::fromFloatNearest(l.val() * rootFont.capHeight());

    case Length::Unit::CH:
        return Px::fromFloatNearest(l.val() * fragFont.zeroAdvance());

    case Length::Unit::RCH:
        return Px::fromFloatNearest(l.val() * rootFont.zeroAdvance());

    case Length::Unit::IC:
        return Px::fromFloatNearest(l.val() * fragFont.zeroAdvance());

    case Length::Unit::RIC:
        return Px::fromFloatNearest(l.val() * rootFont.zeroAdvance());

    case Length::Unit::LH:
        return Px::fromFloatNearest(l.val() * fragFont.lineHeight());

    case Length::Unit::RLH:
        return Px::fromFloatNearest(l.val() * rootFont.lineHeight());
    default:
        panic("expected font-relative unit");
    }
}

Px resolve(Tree &t, Frag &f, Length l) {
    if (l.isFontRelative())
        return _resolveFontRelative(t, f, l);
    switch (l.unit()) {

    // Viewport-relative

    // https://drafts.csswg.org/css-values/#vw

    // Equal to 1% of the width of current viewport.
    case Length::Unit::VW:
    case Length::Unit::LVW:
        return Px::fromFloatNearest(l.val() * t.viewport.large.width.cast<f64>() / 100);

    case Length::Unit::SVW:
        return Px::fromFloatNearest(l.val() * t.viewport.small.width.cast<f64>() / 100);

    case Length::Unit::DVW:
        return Px::fromFloatNearest(l.val() * t.viewport.dynamic.width.cast<f64>() / 100);

    // https://drafts.csswg.org/css-values/#vh
    // Equal to 1% of the height of current viewport.
    case Length::Unit::VH:
    case Length::Unit::LVH:
        return Px::fromFloatNearest(l.val() * t.viewport.large.height.cast<f64>() / 100);

    case Length::Unit::SVH:
        return Px::fromFloatNearest(l.val() * t.viewport.small.height.cast<f64>() / 100);

    case Length::Unit::DVH:
        return Px::fromFloatNearest(l.val() * t.viewport.dynamic.height.cast<f64>() / 100);

    // https://drafts.csswg.org/css-values/#vi
    // Equal to 1% of the size of the viewport in the box’s inline axis.
    case Length::Unit::VI:
    case Length::Unit::LVI:
        if (mainAxis(f) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * t.viewport.large.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * t.viewport.large.height.cast<f64>() / 100);
        }

    case Length::Unit::SVI:
        if (mainAxis(f) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * t.viewport.small.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * t.viewport.small.height.cast<f64>() / 100);
        }

    case Length::Unit::DVI:
        if (mainAxis(f) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * t.viewport.dynamic.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * t.viewport.dynamic.height.cast<f64>() / 100);
        }

    // https://drafts.csswg.org/css-values/#vb
    // Equal to 1% of the size of the viewport in the box’s block axis.
    case Length::Unit::VB:
    case Length::Unit::LVB:
        if (crossAxis(f) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * t.viewport.large.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * t.viewport.large.height.cast<f64>() / 100);
        }

    case Length::Unit::SVB:
        if (crossAxis(f) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * t.viewport.small.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * t.viewport.small.height.cast<f64>() / 100);
        }

    case Length::Unit::DVB:
        if (crossAxis(f) == Axis::HORIZONTAL) {
            return Px::fromFloatNearest(l.val() * t.viewport.dynamic.width.cast<f64>() / 100);
        } else {
            return Px::fromFloatNearest(l.val() * t.viewport.dynamic.height.cast<f64>() / 100);
        }

    // https://drafts.csswg.org/css-values/#vmin
    // Equal to the smaller of vw and vh.
    case Length::Unit::VMIN:
    case Length::Unit::LVMIN:
        return min(
            resolve(t, f, Length(l.val(), Length::Unit::VW)),
            resolve(t, f, Length(l.val(), Length::Unit::VH))
        );

    case Length::Unit::SVMIN:
        return min(
            resolve(t, f, Length(l.val(), Length::Unit::SVW)),
            resolve(t, f, Length(l.val(), Length::Unit::SVH))
        );

    case Length::Unit::DVMIN:
        return min(
            resolve(t, f, Length(l.val(), Length::Unit::DVW)),
            resolve(t, f, Length(l.val(), Length::Unit::DVH))
        );

    // https://drafts.csswg.org/css-values/#vmax
    // Equal to the larger of vw and vh.
    case Length::Unit::VMAX:
    case Length::Unit::LVMAX:
        return max(
            resolve(t, f, Length(l.val(), Length::Unit::VW)),
            resolve(t, f, Length(l.val(), Length::Unit::VH))
        );

    case Length::Unit::DVMAX:
        return max(
            resolve(t, f, Length(l.val(), Length::Unit::DVW)),
            resolve(t, f, Length(l.val(), Length::Unit::DVH))
        );

    case Length::Unit::SVMAX:
        return max(
            resolve(t, f, Length(l.val(), Length::Unit::SVW)),
            resolve(t, f, Length(l.val(), Length::Unit::SVH))
        );

    // Absolute
    // https://drafts.csswg.org/css-values/#absolute-lengths
    case Length::Unit::CM:
        return Px::fromFloatNearest(l.val() * t.viewport.dpi.cast<f64>() / 2.54);

    case Length::Unit::MM:
        return Px::fromFloatNearest(l.val() * t.viewport.dpi.cast<f64>() / 25.4);

    case Length::Unit::Q:
        return Px::fromFloatNearest(l.val() * t.viewport.dpi.cast<f64>() / 101.6);

    case Length::Unit::IN:
        return Px::fromFloatNearest(l.val() * t.viewport.dpi.cast<f64>());

    case Length::Unit::PT:
        return Px::fromFloatNearest(l.val() * t.viewport.dpi.cast<f64>() / 72.0);

    case Length::Unit::PC:
        return Px::fromFloatNearest(l.val() * t.viewport.dpi.cast<f64>() / 6.0);

    case Length::Unit::PX:
        return Px::fromFloatNearest(l.val());

    default:
        panic("invalid length unit");
    }
}

template <typename T>
concept Resolvable = requires {
    typename T::Resolved;
};

template <typename T>
using Resolved = Meta::Cond<Resolvable<T>, typename T::Resolved, T>;
static_assert(Resolvable<PercentOr<Length>>);

Px resolve(Tree &t, Frag &f, PercentOr<Length> p, Px relative) {
    if (p.resolved())
        return resolve(t, f, p.value());
    return Px{relative.cast<f64>() * (p.percent().value() / 100.)};
}

template <typename T>
Resolved<T> resolveInfix(typename CalcValue<T>::OpCode op, Resolved<T> lhs, Resolved<T> rhs) {
    switch (op) {
    case CalcValue<T>::OpCode::ADD:
        return lhs + rhs;
    case CalcValue<T>::OpCode::SUBSTRACT:
        return lhs - rhs;
    case CalcValue<T>::OpCode::MULTIPLY:
        return lhs * rhs;
    case CalcValue<T>::OpCode::DIVIDE:
        return lhs / rhs;
    default:
        return lhs;
    }
}

template <typename T>
auto resolve(Tree &t, Frag &f, CalcValue<T> const &p, Px relative) {
    if (p.type == CalcValue<T>::FIXED) {
        return resolve(t, f, p.lhs.template unwrap<T>(), relative);
    } else if (p.type == CalcValue<T>::SINGLE) {
        // TODO: compute result of funtion here with the resolved value
        return resolve(t, f, p.lhs.template unwrap<T>(), relative);
    } else if (p.type == CalcValue<T>::OpType::CALC) {
        auto resolveUnion = Visitor{
            [&](T const &v) {
                return resolve<T>(t, f, v, relative);
            },
            [&](CalcValue<T>::Leaf const &v) {
                return resolve<T>(t, f, *v, relative);
            },
            [&](Number const &v) {
                return Math::i24f8{v};
            },
            [&](None const &) -> Resolved<T> {
                panic("invalid value in calc expression");
            }
        };

        return resolveInfix<T>(
            p.op,
            p.lhs.visit(resolveUnion),
            p.rhs.visit(resolveUnion)
        );
    }

    unreachable();
}

Px resolve(Tree &t, Frag &f, Width w, Px relative) {
    if (w == Width::Type::AUTO)
        return Px{0};
    return resolve(t, f, w.value, relative);
}

Px resolve(Tree &t, Frag &f, FontSize fs) {
    // FIXME: get from user settings
    f64 userFontSizes = 16;

    // FIXME: get from parent
    f64 parentFontSize = userFontSizes;

    switch (fs.named()) {
    case FontSize::XX_SMALL:
        return Px::fromFloatNearest(userFontSizes * 0.5);
    case FontSize::X_SMALL:
        return Px::fromFloatNearest(userFontSizes * 0.75);
    case FontSize::SMALL:
        return Px::fromFloatNearest(userFontSizes * 0.875);
    case FontSize::MEDIUM:
        return Px::fromFloatNearest(userFontSizes);
    case FontSize::LARGE:
        return Px::fromFloatNearest(userFontSizes * 1.125);
    case FontSize::X_LARGE:
        return Px::fromFloatNearest(userFontSizes * 1.25);
    case FontSize::XX_LARGE:
        return Px::fromFloatNearest(userFontSizes * 1.5);

    case FontSize::LARGER:
        return Px::fromFloatNearest(parentFontSize * 1.25);
    case FontSize::SMALLER:
        return Px::fromFloatNearest(parentFontSize * 0.875);

    case FontSize::LENGTH:
        return resolve(t, f, fs.value(), Px{parentFontSize});

    default:
        panic("unimplemented font size");
        break;
    }
}

} // namespace Vaev::Layout
