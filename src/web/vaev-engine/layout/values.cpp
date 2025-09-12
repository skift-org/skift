module;

#include <karm-gfx/font.h>
#include <karm-logger/logger.h>
#include <karm-math/au.h>

export module Vaev.Engine:layout.values;

import :values;

import :layout.base;
import :layout.writing;

namespace Vaev::Layout {

export struct Resolver {
    f64 userFontSize = 16;   /// Font size of the user agent
    f64 parentFontSize = 16; /// Font size of the parent box

    Opt<Gfx::Font> rootFont = NONE;                  /// Font of the root element
    Opt<Gfx::Font> boxFont = NONE;                   /// Font of the current box
    Viewport viewport = {.small = {800_au, 600_au}}; /// Viewport of the current box
    Axis boxAxis = Axis::HORIZONTAL;                 /// Inline axis of the current box

    static Resolver from(Tree const& tree, Box const& box) {
        Au fontSize{16};

        Resolver resolver;
        resolver.rootFont = Gfx::Font{tree.root.style->fontFace, fontSize.cast<f64>()};
        resolver.boxFont = Gfx::Font{box.style->fontFace, fontSize.cast<f64>()};
        resolver.viewport = tree.viewport;
        resolver.boxAxis = mainAxis(box);
        return resolver;
    }

    Resolver inherit(Resolver const& resolver) {
        Resolver copy = resolver;
        copy.boxFont = NONE;
        copy.parentFontSize = resolver.boxFont.unwrap().fontsize;
        return copy;
    }

    Au _resolveFontRelative(Length const& value) {
        if (not boxFont or not rootFont) {
            logWarn("missing font information");
            return 0_au;
        }

        switch (value.unit()) {

        case Length::EM:
            return Au::fromFloatNearest(value.val() * boxFont.unwrap().fontSize());

        case Length::REM:
            return Au::fromFloatNearest(value.val() * rootFont.unwrap().fontSize());

        case Length::EX:
            return Au::fromFloatNearest(value.val() * boxFont.unwrap().xHeight());

        case Length::REX:
            return Au::fromFloatNearest(value.val() * rootFont.unwrap().xHeight());

        case Length::CAP:
            return Au::fromFloatNearest(value.val() * boxFont.unwrap().capHeight());

        case Length::RCAP:
            return Au::fromFloatNearest(value.val() * rootFont.unwrap().capHeight());

        case Length::CH:
            return Au::fromFloatNearest(value.val() * boxFont.unwrap().zeroAdvance());

        case Length::RCH:
            return Au::fromFloatNearest(value.val() * rootFont.unwrap().zeroAdvance());

        case Length::IC:
            return Au::fromFloatNearest(value.val() * boxFont.unwrap().zeroAdvance());

        case Length::RIC:
            return Au::fromFloatNearest(value.val() * rootFont.unwrap().zeroAdvance());

        case Length::LH:
            return Au::fromFloatNearest(value.val() * boxFont.unwrap().lineHeight());

        case Length::RLH:
            return Au::fromFloatNearest(value.val() * rootFont.unwrap().lineHeight());

        default:
            panic("expected font-relative unit");
        }
    }

    Au resolve(Length const& value) {
        if (value.isFontRelative())
            return _resolveFontRelative(value);
        switch (value.unit()) {

        // Viewport-relative

        // https://drafts.csswg.org/css-values/#vw

        // Equal to 1% of the width of current viewport.
        case Length::VW:
        case Length::LVW:
            return Au::fromFloatNearest(value.val() * viewport.large.width.cast<f64>() / 100);

        case Length::SVW:
            return Au::fromFloatNearest(value.val() * viewport.small.width.cast<f64>() / 100);

        case Length::DVW:
            return Au::fromFloatNearest(value.val() * viewport.dynamic.width.cast<f64>() / 100);

        // https://drafts.csswg.org/css-values/#vh
        // Equal to 1% of the height of current viewport.
        case Length::VH:
        case Length::LVH:
            return Au::fromFloatNearest(value.val() * viewport.large.height.cast<f64>() / 100);

        case Length::SVH:
            return Au::fromFloatNearest(value.val() * viewport.small.height.cast<f64>() / 100);

        case Length::DVH:
            return Au::fromFloatNearest(value.val() * viewport.dynamic.height.cast<f64>() / 100);

        // https://drafts.csswg.org/css-values/#vi
        // Equal to 1% of the size of the viewport in the box’s inline axis.
        case Length::VI:
        case Length::LVI:
            if (boxAxis == Axis::HORIZONTAL) {
                return Au::fromFloatNearest(value.val() * viewport.large.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(value.val() * viewport.large.height.cast<f64>() / 100);
            }

        case Length::SVI:
            if (boxAxis == Axis::HORIZONTAL) {
                return Au::fromFloatNearest(value.val() * viewport.small.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(value.val() * viewport.small.height.cast<f64>() / 100);
            }

        case Length::DVI:
            if (boxAxis == Axis::HORIZONTAL) {
                return Au::fromFloatNearest(value.val() * viewport.dynamic.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(value.val() * viewport.dynamic.height.cast<f64>() / 100);
            }

        // https://drafts.csswg.org/css-values/#vb
        // Equal to 1% of the size of the viewport in the box’s block axis.
        case Length::VB:
        case Length::LVB:
            if (boxAxis.cross() == Axis::HORIZONTAL) {
                return Au::fromFloatNearest(value.val() * viewport.large.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(value.val() * viewport.large.height.cast<f64>() / 100);
            }

        case Length::SVB:
            if (boxAxis.cross() == Axis::HORIZONTAL) {
                return Au::fromFloatNearest(value.val() * viewport.small.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(value.val() * viewport.small.height.cast<f64>() / 100);
            }

        case Length::DVB:
            if (boxAxis.cross() == Axis::HORIZONTAL) {
                return Au::fromFloatNearest(value.val() * viewport.dynamic.width.cast<f64>() / 100);
            } else {
                return Au::fromFloatNearest(value.val() * viewport.dynamic.height.cast<f64>() / 100);
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
            return Au::fromFloatNearest(value.val() * 96 / 2.54);

        case Length::MM:
            return Au::fromFloatNearest(value.val() * 96 / 25.4);

        case Length::Q:
            return Au::fromFloatNearest(value.val() * 96 / 101.6);

        case Length::IN:
            return Au::fromFloatNearest(value.val() * 96);

        case Length::PT:
            return Au::fromFloatNearest(value.val() * 96 / 72.0);

        case Length::PC:
            return Au::fromFloatNearest(value.val() * 96 / 6.0);

        case Length::PX:
            return Au::fromFloatNearest(value.val());

        default:
            panic("invalid length unit");
        }
    }

    Au resolve(LineWidth const& value) {
        return value.visit(Visitor{
            [](Keywords::Thin const&) {
                return THIN_VALUE;
            },
            [](Keywords::Medium const&) {
                return MEDIUM_VALUE;
            },
            [](Keywords::Thick const&) {
                return THICK_VALUE;
            },
            [&](auto const& length) {
                return resolve(length);
            },
        });
    }

    Au resolve(PercentOr<Length> const& value, Au relative) {
        if (auto valueLength = value.is<Length>())
            return resolve(*valueLength);
        return Au{relative.cast<f64>() * (value.unwrap<Percent>().value() / 100.)};
    }

    Au resolve(Width const& value, Au relative) {
        if (value.is<Keywords::Auto>())
            return 0_au;
        return resolve(value.unwrap<CalcValue<PercentOr<Length>>>(), relative);
    }

    Rad resolve(Angle const& value) {
        return Rad{value.toRadian()};
    }

    Number resolve(Number const& value) {
        return value;
    }

    Au resolve(FontSize const& value) {
        switch (value.named()) {
        case FontSize::XX_SMALL:
            return Au::fromFloatNearest(userFontSize * 0.5);
        case FontSize::X_SMALL:
            return Au::fromFloatNearest(userFontSize * 0.75);
        case FontSize::SMALL:
            return Au::fromFloatNearest(userFontSize * 0.875);
        case FontSize::MEDIUM:
            return Au::fromFloatNearest(userFontSize);
        case FontSize::LARGE:
            return Au::fromFloatNearest(userFontSize * 1.125);
        case FontSize::X_LARGE:
            return Au::fromFloatNearest(userFontSize * 1.25);
        case FontSize::XX_LARGE:
            return Au::fromFloatNearest(userFontSize * 1.5);

        case FontSize::LARGER:
            return Au::fromFloatNearest(parentFontSize * 1.25);
        case FontSize::SMALLER:
            return Au::fromFloatNearest(parentFontSize * 0.875);

        case FontSize::LENGTH:
            return resolve(value.value(), Au{parentFontSize});

        default:
            panic("unimplemented font size");
            break;
        }
    }

    // MARK: Eval --------------------------------------------------------------

    template <typename T>
    Resolved<T> _resolveUnary(CalcOp, Resolved<T>) {
        notImplemented();
    }

    template <typename T>
    Resolved<T> _resolveInfix(CalcOp op, Resolved<T> lhs, Resolved<T> rhs) {
        switch (op) {
        case CalcOp::ADD:
            return lhs + rhs;
        case CalcOp::SUBSTRACT:
            return lhs - rhs;
        case CalcOp::MULTIPLY:
            return lhs * rhs;
        case CalcOp::DIVIDE:
            return lhs / rhs;
        default:
            panic("unexpected operator");
        }
    }

    template <typename T, typename... Args>
    Resolved<T> resolve(CalcValue<T> const& calc, Args... args) {
        auto resolveUnion = Visitor{
            [&](T const& v) {
                return resolve(v, args...);
            },
            [&](CalcValue<T>::Leaf const& v) {
                return resolve<T>(*v, args...);
            },
            [&](Number const& v)
                requires(not Meta::Same<T, Number>)
            {
                return Resolved<T>{v};
            }
            };

        return calc.visit(Visitor{
            [&](typename CalcValue<T>::Value const& v) {
                return v.visit(resolveUnion);
            },
            [&](typename CalcValue<T>::Unary const& u) {
                return _resolveUnary<T>(
                    u.op,
                    u.val.visit(resolveUnion)
                );
            },
            [&](typename CalcValue<T>::Binary const& b) {
                return _resolveInfix<T>(
                    b.op,
                    b.lhs.visit(resolveUnion),
                    b.rhs.visit(resolveUnion)
                );
            },
        });
    }
};

// MARK: Resolve during layout -------------------------------------------------

// HACK: Temporary workaround while we don't properly evaluate computed values
export bool isPurePercentage(CalcValue<PercentOr<Length>> calcValue) {
    if (not calcValue._inner.is<CalcValue<PercentOr<Length>>::Value>())
        return false;

    auto const& value = calcValue._inner.unwrap<CalcValue<PercentOr<Length>>::Value>();
    if (not value.is<PercentOr<Length>>())
        return false;

    return value.unwrap<PercentOr<Length>>().is<Percent>();
}

export Au resolve(Tree const& tree, Box const& box, Length const& value) {
    return Resolver::from(tree, box).resolve(value);
}

export Au resolve(Tree const& tree, Box const& box, PercentOr<Length> const& value, Au relative) {
    return Resolver::from(tree, box).resolve(value, relative);
}

export Au resolve(Tree const& tree, Box const& box, Width const& value, Au relative) {
    return Resolver::from(tree, box).resolve(value, relative);
}

export Au resolve(Tree const& tree, Box const& box, LineWidth const& value) {
    return Resolver::from(tree, box).resolve(value);
}

export Au resolve(Tree const& tree, Box const& box, FontSize const& value) {
    return Resolver::from(tree, box).resolve(value);
}

export template <typename T, typename... Args>
auto resolve(Tree const& tree, Box const& box, CalcValue<T> const& value, Args... args) -> Resolved<T> {
    return Resolver::from(tree, box).resolve(value, args...);
}

} // namespace Vaev::Layout
