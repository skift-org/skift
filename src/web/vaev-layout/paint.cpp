#include <karm-scene/box.h>
#include <karm-scene/text.h>

#include "paint.h"

namespace Vaev::Layout {

static bool _paintBorders(Frag &frag, Gfx::Color currentColor, Gfx::Borders &borders) {
    currentColor = resolve(frag.style->color, currentColor);

    borders.radii = frag.layout.radii.cast<f64>();

    auto bordersLayout = frag.layout.borders;
    borders.widths.top = bordersLayout.top.cast<f64>();
    borders.widths.bottom = bordersLayout.bottom.cast<f64>();
    borders.widths.start = bordersLayout.start.cast<f64>();
    borders.widths.end = bordersLayout.end.cast<f64>();

    auto bordersStyle = frag.style->borders;
    borders.styles[0] = bordersStyle->top.style;
    borders.styles[1] = bordersStyle->end.style;
    borders.styles[2] = bordersStyle->bottom.style;
    borders.styles[3] = bordersStyle->start.style;

    borders.fills[0] = resolve(bordersStyle->top.color, currentColor);
    borders.fills[1] = resolve(bordersStyle->end.color, currentColor);
    borders.fills[2] = resolve(bordersStyle->bottom.color, currentColor);
    borders.fills[3] = resolve(bordersStyle->start.color, currentColor);

    return not borders.widths.zero();
}

static void _paintBox(Frag &frag, Gfx::Color currentColor, Scene::Stack &stack) {
    auto const &backgrounds = frag.style->backgrounds;

    Scene::Box paint;
    bool hasBackgrounds = any(backgrounds);

    if (hasBackgrounds) {
        paint.backgrounds.ensure(backgrounds.len());
        for (auto &bg : backgrounds) {
            auto color = resolve(bg.fill, currentColor);

            // Skip transparent backgrounds
            if (color.alpha == 0)
                continue;

            paint.backgrounds.pushBack(color);
        }
    }

    bool hasBorders = _paintBorders(frag, currentColor, paint.borders);
    paint.bound = frag.layout.borderBox().cast<f64>();

    if (hasBackgrounds or hasBorders)
        stack.add(makeStrong<Scene::Box>(std::move(paint)));
}

static void _establishStackingContext(Frag &frag, Scene::Stack &stack);
static void _paintStackingContext(Frag &frag, Scene::Stack &stack);

static void _paintFrag(Frag &frag, Scene::Stack &stack) {
    Gfx::Color currentColor = Gfx::BLACK;
    currentColor = resolve(frag.style->color, currentColor);

    _paintBox(frag, currentColor, stack);

    if (auto run = frag.content.is<Karm::Text::Run>()) {
        Karm::Text::Font font = {frag.fontFace, frag.layout.fontSize.cast<f64>()};
        Math::Vec2f baseline = {0, font.metrics().ascend};
        stack.add(makeStrong<Scene::Text>(
            frag.layout.borderBox().topStart().cast<f64>() + baseline,
            font,
            *run,
            currentColor
        ));
    }
}

static void _paintChildren(Frag &frag, Scene::Stack &stack, auto predicate) {
    for (auto &c : frag.children()) {
        auto &s = *c.style;

        auto zIndex = s.zIndex;
        if (zIndex != ZIndex::AUTO) {
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

static void _paintStackingContext(Frag &frag, Scene::Stack &stack) {
    // 1. the background and borders of the element forming the stacking context.
    _paintFrag(frag, stack);

    // 2. the child stacking contexts with negative stack levels (most negative first).
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex.value < 0;
    });

    // 3. the in-flow, non-inline-level, non-positioned descendants.
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex == ZIndex::AUTO and s.display != Display::INLINE and s.position == Position::STATIC;
    });

    // 4. the non-positioned floats.
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex == ZIndex::AUTO and s.position == Position::STATIC and s.float_ != Float::NONE;
    });

    // 5. the in-flow, inline-level, non-positioned descendants, including inline tables and inline blocks.
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex == ZIndex::AUTO and s.display == Display::INLINE and s.position == Position::STATIC;
    });

    // 6. the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex.value == 0 and s.position != Position::STATIC;
    });

    // 7. the child stacking contexts with positive stack levels (least positive first).
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex.value > 0;
    });
}

static void _establishStackingContext(Frag &frag, Scene::Stack &stack) {
    auto innerStack = makeStrong<Scene::Stack>();
    innerStack->zIndex = frag.style->zIndex.value;
    _paintStackingContext(frag, *innerStack);
    stack.add(std::move(innerStack));
}

void paint(Frag &frag, Scene::Stack &stack) {
    _paintStackingContext(frag, stack);
}

void wireframe(Frag &frag, Gfx::Canvas &g) {
    for (auto &c : frag.children())
        wireframe(c, g);

    g.strokeStyle({
        .fill = Gfx::BLACK,
        .width = 1,
        .align = Gfx::INSIDE_ALIGN,
    });

    g.stroke(frag.layout.borderBox().cast<f64>());
}

} // namespace Vaev::Layout
