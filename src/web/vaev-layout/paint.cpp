module;

#include <karm-scene/box.h>
#include <karm-scene/clip.h>
#include <karm-scene/image.h>
#include <karm-scene/text.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:paint;

import :base;
import :values;

namespace Vaev::Layout {

static bool _paintBorders(Frag& frag, Gfx::Color currentColor, Gfx::Borders& borders) {
    borders.radii = frag.metrics.radii.cast<f64>(); // This value is needed for the outline

    if (frag.metrics.borders.zero())
        return false;

    currentColor = Vaev::resolve(frag.style().color, currentColor);

    auto const& bordersLayout = frag.metrics.borders;
    borders.widths.top = bordersLayout.top.cast<f64>();
    borders.widths.bottom = bordersLayout.bottom.cast<f64>();
    borders.widths.start = bordersLayout.start.cast<f64>();
    borders.widths.end = bordersLayout.end.cast<f64>();

    auto const& bordersStyle = *frag.style().borders;
    borders.styles[0] = bordersStyle.top.style;
    borders.styles[1] = bordersStyle.end.style;
    borders.styles[2] = bordersStyle.bottom.style;
    borders.styles[3] = bordersStyle.start.style;

    borders.fills[0] = Vaev::resolve(bordersStyle.top.color, currentColor);
    borders.fills[1] = Vaev::resolve(bordersStyle.end.color, currentColor);
    borders.fills[2] = Vaev::resolve(bordersStyle.bottom.color, currentColor);
    borders.fills[3] = Vaev::resolve(bordersStyle.start.color, currentColor);

    return true;
}

static bool _paintOutline(Frag& frag, Gfx::Color currentColor, Gfx::Outline& outline) {
    if (frag.metrics.outlineWidth == 0_au)
        return false;

    outline.width = frag.metrics.outlineWidth.cast<f64>();
    outline.offset = frag.metrics.outlineOffset.cast<f64>();

    auto const& outlineStyle = *frag.style().outline;
    if (outlineStyle.style.is<Keywords::Auto>()) {
        outline.style = Gfx::BorderStyle::SOLID;
    } else {
        outline.style = outlineStyle.style.unwrap<Gfx::BorderStyle>();
    }

    if (outlineStyle.color.is<Keywords::Auto>()) {
        if (outlineStyle.style.is<Keywords::Auto>()) {
            outline.fill = resolve(Color(SystemColor::ACCENT_COLOR), currentColor);
        } else {
            outline.fill = currentColor;
        }
    } else {
        outline.fill = resolve(outlineStyle.color.unwrap<Color>(), currentColor);
    }

    return true;
}

static bool _needsNewStackingContext(Frag const& frag) {
    return frag.style().zIndex != Keywords::AUTO or frag.style().clip.has();
}

static void _paintFragBordersAndBackgrounds(Frag& frag, Scene::Stack& stack) {
    auto const& cssBackground = frag.style().backgrounds;

    Gfx::Borders borders;
    Gfx::Outline outline;

    Vec<Gfx::Fill> backgrounds;
    auto color = Vaev::resolve(cssBackground->color, frag.style().color);
    if (color.alpha != 0)
        backgrounds.pushBack(color);

    auto currentColor = Vaev::resolve(frag.style().color, color);
    bool hasBorders = _paintBorders(frag, currentColor, borders);
    bool hasOutline = _paintOutline(frag, currentColor, outline);
    Math::Rectf bound = frag.metrics.borderBox().round().cast<f64>();

    if (any(backgrounds) or hasBorders or hasOutline) {
        auto box = makeRc<Scene::Box>(bound, std::move(borders), std::move(outline), std::move(backgrounds));
        box->zIndex = _needsNewStackingContext(frag) ? Limits<isize>::MIN : 0;
        stack.add(box);
    }
}

static void _establishStackingContext(Frag& frag, Scene::Stack& stack);
static void _paintStackingContext(Frag& frag, Scene::Stack& stack);

static void _paintFrag(Frag& frag, Scene::Stack& stack) {
    auto& s = frag.style();
    if (s.visibility == Visibility::HIDDEN)
        return;

    _paintFragBordersAndBackgrounds(frag, stack);

    if (auto ic = frag.box->content.is<InlineBox>()) {
        stack.add(makeRc<Scene::Text>(frag.metrics.contentBox().topStart().cast<f64>(), ic->prose));
    } else if (auto image = frag.box->content.is<Karm::Image::Picture>()) {
        stack.add(makeRc<Scene::Image>(frag.metrics.borderBox().cast<f64>(), *image));
    }
}

static void _paintChildren(Frag& frag, Scene::Stack& stack, auto predicate) {
    for (auto& c : frag.children) {
        auto& s = c.style();

        if (_needsNewStackingContext(c)) {
            if (predicate(s))
                _establishStackingContext(c, stack);
            continue;
        }

        // NOTE: Positioned elements act as if they establish a stacking context
        auto position = s.position;
        if (position != Position::STATIC) {
            if (predicate(s))
                _paintStackingContext(c, stack);
            continue;
        }

        if (predicate(s))
            _paintFrag(c, stack);
        _paintChildren(c, stack, predicate);
    }
}

static void _paintStackingContext(Frag& frag, Scene::Stack& stack) {
    // 1. the background and borders of the element forming the stacking context.
    _paintFrag(frag, stack);

    // 2. the child stacking contexts with negative stack levels (most negative first).
    _paintChildren(frag, stack, [](Style::ComputedStyle const& s) -> bool {
        return s.zIndex.unwrapOr<isize>(0) < 0;
    });

    // 3. the in-flow, non-inline-level, non-positioned descendants.
    _paintChildren(frag, stack, [](Style::ComputedStyle const& s) {
        return s.zIndex == Keywords::AUTO and s.display != Display::INLINE and s.position == Position::STATIC;
    });

    // 4. the non-positioned floats.
    _paintChildren(frag, stack, [](Style::ComputedStyle const& s) {
        return s.zIndex == Keywords::AUTO and s.position == Position::STATIC and s.float_ != Float::NONE;
    });

    // 5. the in-flow, inline-level, non-positioned descendants, including inline tables and inline blocks.
    _paintChildren(frag, stack, [](Style::ComputedStyle const& s) {
        return s.zIndex == Keywords::AUTO and s.display == Display::INLINE and s.position == Position::STATIC;
    });

    // 6. the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.
    _paintChildren(frag, stack, [](Style::ComputedStyle const& s) {
        return s.zIndex.unwrapOr<isize>(0) == 0 and s.position != Position::STATIC;
    });

    // 7. the child stacking contexts with positive stack levels (least positive first).
    _paintChildren(frag, stack, [](Style::ComputedStyle const& s) {
        return s.zIndex.unwrapOr<isize>(0) > 0;
    });
}

static Math::Vec2f _resolveBackgroundPosition(Resolver& resolver, BackgroundPosition const& position, RectAu const& referenceBox) {
    Math::Vec2f result;

    if (position.horizontalAnchor.is<Keywords::Left>()) {
        result.x = resolver.resolve(position.horizontal, referenceBox.width).cast<f64>();
    } else if (position.horizontalAnchor.is<Keywords::Right>()) {
        result.x = (referenceBox.width - resolver.resolve(position.horizontal, referenceBox.width)).cast<f64>();
    } else if (position.horizontalAnchor.is<Keywords::Center>()) {
        result.x = referenceBox.width.cast<f64>() / 2.0;
    }

    if (position.verticalAnchor.is<Keywords::Top>()) {
        result.y = resolver.resolve(position.vertical, referenceBox.height).cast<f64>();
    } else if (position.verticalAnchor.is<Keywords::Bottom>()) {
        result.y = (referenceBox.height - resolver.resolve(position.vertical, referenceBox.height)).cast<f64>();
    } else if (position.verticalAnchor.is<Keywords::Center>()) {
        result.y = referenceBox.height.cast<f64>() / 2.0;
    }

    return result;
}

static Math::Radiif _resolveRadii(Resolver& resolver, Math::Radii<CalcValue<PercentOr<Length>>> const& baseRadii, RectAu const& referenceBox) {
    Math::Radiif radii;
    radii.a = resolver.resolve(baseRadii.a, referenceBox.height).cast<f64>();
    radii.b = resolver.resolve(baseRadii.b, referenceBox.width).cast<f64>();
    radii.c = resolver.resolve(baseRadii.c, referenceBox.width).cast<f64>();
    radii.d = resolver.resolve(baseRadii.d, referenceBox.height).cast<f64>();
    radii.e = resolver.resolve(baseRadii.e, referenceBox.height).cast<f64>();
    radii.f = resolver.resolve(baseRadii.f, referenceBox.width).cast<f64>();
    radii.g = resolver.resolve(baseRadii.g, referenceBox.width).cast<f64>();
    radii.h = resolver.resolve(baseRadii.h, referenceBox.height).cast<f64>();
    return radii;
}

static Rc<Scene::Clip> _resolveClip(Frag const& frag) {
    Math::Path result;
    auto& clip = frag.style().clip.unwrap();

    // TODO: handle SVG cases (https://drafts.fxtf.org/css-masking/#typedef-geometry-box)
    auto [referenceBox, radii] = clip.referenceBox.visit(Visitor{
        [&](Keywords::BorderBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.borderBox(), frag.metrics.radii};
        },
        [&](Keywords::PaddingBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.paddingBox(), {0_au}};
        },
        [&](Keywords::ContentBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.contentBox(), {0_au}};
        },
        [&](Keywords::MarginBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.marginBox(), {0_au}};
        },
        [&](Keywords::FillBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.contentBox(), {0_au}};
        },
        [&](Keywords::StrokeBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.borderBox(), frag.metrics.radii};
        },
        [&](Keywords::ViewBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.borderBox(), {0_au}};
        },
    });

    if (not clip.shape) {
        result.rect(referenceBox.round().cast<f64>(), radii.cast<f64>());
        return makeRc<Scene::Clip>(result);
    }

    auto resolver = Resolver();
    return clip.shape.unwrap().visit(Visitor{
        [&](Polygon const& polygon) {
            result.moveTo(
                referenceBox.xy.cast<f64>() +
                Math::Vec2f(
                    resolver.resolve(first(polygon.points).v0, referenceBox.width).cast<f64>(),
                    resolver.resolve(first(polygon.points).v1, referenceBox.height).cast<f64>()
                )
            );
            for (auto& point : next(polygon.points)) {
                result.lineTo(
                    referenceBox.xy.cast<f64>() +
                    Math::Vec2f(
                        resolver.resolve(point.v0, referenceBox.width).cast<f64>(),
                        resolver.resolve(point.v1, referenceBox.height).cast<f64>()
                    )
                );
            }

            return makeRc<Scene::Clip>(result, polygon.fillRule);
        },
        [&](Circle const& circle) {
            auto center = _resolveBackgroundPosition(resolver, circle.position, referenceBox);
            f64 radius;
            if (circle.radius.is<Keywords::ClosestSide>()) {
                radius = min(
                    Math::abs(referenceBox.width.cast<f64>() - center.x),
                    center.x,
                    center.y,
                    Math::abs(referenceBox.height.cast<f64>() - center.y)
                );
            } else if (circle.radius.is<Keywords::FarthestSide>()) {
                radius = max(
                    Math::abs(referenceBox.width.cast<f64>() - center.x),
                    center.x,
                    center.y,
                    Math::abs(referenceBox.height.cast<f64>() - center.y)
                );
            } else {
                auto hSquared = Math::pow2(referenceBox.height.cast<f64>());
                auto wSquared = Math::pow2(referenceBox.width.cast<f64>());
                radius = resolver.resolve(
                                     circle.radius.unwrap<CalcValue<PercentOr<Length>>>(),
                                     Au(Math::sqrt(hSquared + wSquared) / Math::sqrt(2.0))
                )
                             .cast<f64>();
            }
            result.ellipse(Math::Ellipsef(center + referenceBox.xy.cast<f64>(), radius));

            return makeRc<Scene::Clip>(result);
        },
        [&](Inset const& inset) {
            Math::Insetsf resolved;
            resolved.start = resolver.resolve(inset.insets.start, referenceBox.width).cast<f64>();
            resolved.end = resolver.resolve(inset.insets.end, referenceBox.width).cast<f64>();
            resolved.top = resolver.resolve(inset.insets.top, referenceBox.height).cast<f64>();
            resolved.bottom = resolver.resolve(inset.insets.bottom, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = _resolveRadii(resolver, inset.borderRadius, referenceBox);

            result.rect(referenceBox.cast<f64>().shrink(resolved), resolvedRadii);

            return makeRc<Scene::Clip>(result);
        },
        [&](Xywh const& xywh) {
            Math::Rectf resolvedRect;
            resolvedRect.x = resolver.resolve(xywh.rect.x, referenceBox.width).cast<f64>();
            resolvedRect.y = resolver.resolve(xywh.rect.y, referenceBox.height).cast<f64>();
            resolvedRect.width = resolver.resolve(xywh.rect.width, referenceBox.width).cast<f64>();
            resolvedRect.height = resolver.resolve(xywh.rect.height, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = _resolveRadii(resolver, xywh.borderRadius, referenceBox);

            result.rect(resolvedRect.offset(referenceBox.xy.cast<f64>()), resolvedRadii);

            return makeRc<Scene::Clip>(result);
        },
        [&](Rect const& rect) {
            Math::Insetsf resolvedInsets;
            resolvedInsets.top = resolver.resolve(rect.insets.top, referenceBox.height).cast<f64>();
            resolvedInsets.end = resolver.resolve(rect.insets.end, referenceBox.width).cast<f64>();
            resolvedInsets.bottom = resolver.resolve(rect.insets.bottom, referenceBox.height).cast<f64>();
            resolvedInsets.start = resolver.resolve(rect.insets.start, referenceBox.width).cast<f64>();

            Math::Radiif resolvedRadii = _resolveRadii(resolver, rect.borderRadius, referenceBox);

            auto resultBox = referenceBox.cast<f64>();
            resultBox.width = max(resolvedInsets.end - resolvedInsets.start, 0);
            resultBox.height = max(resolvedInsets.bottom - resolvedInsets.top, 0);
            resultBox.x += resolvedInsets.start;
            resultBox.y += resolvedInsets.top;

            result.rect(resultBox, resolvedRadii);

            return makeRc<Scene::Clip>(result);
        },
        [&](Ellipse const& ellipse) {
            auto center = _resolveBackgroundPosition(resolver, ellipse.position, referenceBox);

            f64 rx;
            if (ellipse.rx.is<Keywords::ClosestSide>()) {
                rx = min(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else if (ellipse.rx.is<Keywords::FarthestSide>()) {
                rx = max(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else {
                rx = resolver.resolve(
                                 ellipse.rx.unwrap<CalcValue<PercentOr<Length>>>(),
                                 referenceBox.width
                )
                         .cast<f64>();
            }

            f64 ry;
            if (ellipse.ry.is<Keywords::ClosestSide>()) {
                ry = min(Math::abs(referenceBox.height.cast<f64>() - center.y), center.y);
            } else if (ellipse.ry.is<Keywords::FarthestSide>()) {
                ry = max(Math::abs(referenceBox.height.cast<f64>() - center.y), center.y);
            } else {
                ry = resolver.resolve(
                                 ellipse.ry.unwrap<CalcValue<PercentOr<Length>>>(),
                                 referenceBox.height
                )
                         .cast<f64>();
            }
            result.ellipse(Math::Ellipsef(center + referenceBox.xy.cast<f64>(), Math::Vec2f(rx, ry)));

            return makeRc<Scene::Clip>(result);
        },
        [&](Path const& path) {
            result.path(path.path);
            result.offset(referenceBox.xy.cast<f64>());
            return makeRc<Scene::Clip>(result, path.fillRule);
        },
    });
}

static void _establishStackingContext(Frag& frag, Scene::Stack& stack) {
    Rc<Scene::Stack> innerStack = frag.style().clip.has() ? _resolveClip(frag) : makeRc<Scene::Stack>();
    innerStack->zIndex = frag.style().zIndex.unwrapOr<isize>(0);
    _paintStackingContext(frag, *innerStack);
    stack.add(std::move(innerStack));
}

export void paint(Frag& frag, Scene::Stack& stack) {
    _paintStackingContext(frag, stack);
}

export void wireframe(Frag& frag, Gfx::Canvas& g) {
    for (auto& c : frag.children)
        wireframe(c, g);

    g.strokeStyle({
        .fill = Gfx::GREEN,
        .width = 1,
        .align = Gfx::INSIDE_ALIGN,
    });

    g.stroke(frag.metrics.borderBox().cast<f64>());
}

export void overlay(Frag& frag, Gfx::Canvas& g, Gc::Ref<Dom::Node> node) {
    if (frag.box->origin == node) {
        Gfx::Borders border;

        // Margins
        border.widths = frag.metrics.margin.cast<f64>();
        border.withFill(Gfx::YELLOW800.withOpacity(0.5));
        border.withStyle(Gfx::BorderStyle::SOLID);
        border.paint(g, frag.metrics.marginBox().cast<f64>());

        // Borders
        border.widths = frag.metrics.borders.cast<f64>();
        border.withFill(Gfx::YELLOW500.withOpacity(0.5));
        border.withStyle(Gfx::BorderStyle::SOLID);
        border.paint(g, frag.metrics.borderBox().cast<f64>());

        // Paddings
        border.widths = frag.metrics.padding.cast<f64>();
        border.withFill(Gfx::GREEN500.withOpacity(0.5));
        border.withStyle(Gfx::BorderStyle::SOLID);
        border.paint(g, frag.metrics.paddingBox().cast<f64>());

        // Content Box
        g.fillStyle(Gfx::BLUE.withOpacity(0.5));
        g.fill(frag.metrics.contentBox().cast<f64>());
    }

    for (auto& c : frag.children)
        overlay(c, g, node);
}

} // namespace Vaev::Layout
