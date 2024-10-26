#include "layout.h"

#include "block.h"
#include "flex.h"
#include "frag.h"
#include "grid.h"
#include "table.h"
#include "values.h"

namespace Vaev::Layout {

Output _contentLayout(Tree &t, Frag &f, Input input) {
    auto display = f.style->display;

    if (auto run = f.content.is<Karm::Text::Run>()) {
        f.layout.fontSize = resolve(t, f, f.style->font->size);
        Karm::Text::Font font = {
            f.fontFace,
            f.layout.fontSize.cast<f64>(),
        };
        run->shape(font);
        return Output::fromSize(run->size().cast<Px>());
    } else if (
        display == Display::FLOW or
        display == Display::FLOW_ROOT or
        display == Display::TABLE_CELL or
        display == Display::TABLE_CAPTION
    ) {
        return blockLayout(t, f, input);
    } else if (display == Display::FLEX) {
        return flexLayout(t, f, input);
    } else if (display == Display::GRID) {
        return gridLayout(t, f, input);
    } else if (display == Display::TABLE) {
        return tableLayout(t, f, input);
    } else if (display == Display::INTERNAL) {
        return Output{};
    } else {
        return blockLayout(t, f, input);
    }
}

InsetsPx computeMargins(Tree &t, Frag &f, Input input) {
    InsetsPx res;
    auto margin = f.style->margin;

    res.top = resolve(t, f, margin->top, input.containingBlock.height);
    res.end = resolve(t, f, margin->end, input.containingBlock.width);
    res.bottom = resolve(t, f, margin->bottom, input.containingBlock.height);
    res.start = resolve(t, f, margin->start, input.containingBlock.width);

    return res;
}

InsetsPx computeBorders(Tree &t, Frag &f) {
    InsetsPx res;
    auto borders = f.style->borders;

    if (borders->top.style != Gfx::BorderStyle::NONE)
        res.top = resolve(t, f, borders->top.width);

    if (borders->end.style != Gfx::BorderStyle::NONE)
        res.end = resolve(t, f, borders->end.width);

    if (borders->bottom.style != Gfx::BorderStyle::NONE)
        res.bottom = resolve(t, f, borders->bottom.width);

    if (borders->start.style != Gfx::BorderStyle::NONE)
        res.start = resolve(t, f, borders->start.width);

    return res;
}

static InsetsPx _computePaddings(Tree &t, Frag &f, Input input) {
    InsetsPx res;
    auto padding = f.style->padding;

    res.top = resolve(t, f, padding->top, input.containingBlock.height);
    res.end = resolve(t, f, padding->end, input.containingBlock.width);
    res.bottom = resolve(t, f, padding->bottom, input.containingBlock.height);
    res.start = resolve(t, f, padding->start, input.containingBlock.width);

    return res;
}

static Math::Radii<Px> _computeRadii(Tree &t, Frag &f, Vec2Px size) {
    auto radii = f.style->borders->radii;
    Math::Radii<Px> res;

    res.a = resolve(t, f, radii.a, size.height);
    res.b = resolve(t, f, radii.b, size.width);
    res.c = resolve(t, f, radii.c, size.width);
    res.d = resolve(t, f, radii.d, size.height);
    res.e = resolve(t, f, radii.e, size.height);
    res.f = resolve(t, f, radii.f, size.width);
    res.g = resolve(t, f, radii.g, size.width);
    res.h = resolve(t, f, radii.h, size.height);

    return res;
}

static Cons<Opt<Px>, IntrinsicSize> _computeSpecifiedSize(Tree &t, Frag &f, Input input, Size size, IntrinsicSize intrinsic) {
    if (size == Size::MIN_CONTENT or intrinsic == IntrinsicSize::MIN_CONTENT) {
        return {NONE, IntrinsicSize::MIN_CONTENT};
    } else if (size == Size::MAX_CONTENT or intrinsic == IntrinsicSize::MAX_CONTENT) {
        return {NONE, IntrinsicSize::MAX_CONTENT};
    } else if (size == Size::AUTO) {
        return {NONE, IntrinsicSize::AUTO};
    } else if (size == Size::FIT_CONTENT) {
        return {NONE, IntrinsicSize::STRETCH_TO_FIT};
    } else if (size == Size::LENGTH) {
        return {resolve(t, f, size.value, input.containingBlock.width), IntrinsicSize::AUTO};
    } else {
        logWarn("unknown specified size: {}", size);
        return {Px{0}, IntrinsicSize::AUTO};
    }
}

Output layout(Tree &t, Frag &f, Input input) {
    // FIXME: confirm how the preffered width/height parameters interacts with intrinsic size argument from input
    auto borders = computeBorders(t, f);
    auto padding = _computePaddings(t, f, input);
    auto sizing = f.style->sizing;

    auto [specifiedWidth, widthIntrinsicSize] = _computeSpecifiedSize(t, f, input, sizing->width, input.intrinsic.x);
    if (input.knownSize.width == NONE) {
        // FIXME: making prefered width as mandatory width; im not sure this is ok
        input.knownSize.width = specifiedWidth;
    }
    input.knownSize.width = input.knownSize.width.map([&](auto s) {
        if (f.style->boxSizing == BoxSizing::CONTENT_BOX)
            return s;
        return s - padding.horizontal() - borders.horizontal();
    });
    input.intrinsic.x = widthIntrinsicSize;

    auto [specifiedHeight, heightIntrinsicSize] = _computeSpecifiedSize(t, f, input, sizing->height, input.intrinsic.y);
    if (input.knownSize.height == NONE) {
        input.knownSize.height = specifiedHeight;
    }

    input.knownSize.height = input.knownSize.height.map([&](auto s) {
        if (f.style->boxSizing == BoxSizing::CONTENT_BOX)
            return s;
        return s - padding.vertical() - borders.vertical();
    });
    input.intrinsic.y = heightIntrinsicSize;

    input.position = input.position + borders.topStart() + padding.topStart();

    auto [size] = _contentLayout(t, f, input);

    size.width = input.knownSize.width.unwrapOr(size.width);
    size.height = input.knownSize.height.unwrapOr(size.height);

    size = size + padding.all() + borders.all();

    if (input.commit == Commit::YES) {
        f.layout.position = input.position - borders.topStart() - padding.topStart();
        f.layout.borderSize = size;
        f.layout.padding = padding;
        f.layout.borders = borders;
        f.layout.radii = _computeRadii(t, f, size);
    }

    return Output::fromSize(size);
}

} // namespace Vaev::Layout
