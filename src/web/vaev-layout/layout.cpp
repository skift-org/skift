#include "layout.h"

#include "block.h"
#include "box.h"
#include "flex.h"
#include "grid.h"
#include "inline.h"
#include "table.h"
#include "values.h"

namespace Vaev::Layout {

Output _contentLayout(Tree &tree, Box &box, Input input) {
    auto display = box.style->display;

    if (auto image = box.content.is<Karm::Image::Picture>()) {
        return Output::fromSize(image->bound().size().cast<Px>());
    } else if (auto run = box.content.is<Strong<Text::Prose>>()) {
        return inlineLayout(tree, box, input);
    } else if (
        display == Display::FLOW or
        display == Display::FLOW_ROOT or
        display == Display::TABLE_CELL or
        display == Display::TABLE_CAPTION
    ) {
        return blockLayout(tree, box, input);
    } else if (display == Display::FLEX) {
        return flexLayout(tree, box, input);
    } else if (display == Display::GRID) {
        return gridLayout(tree, box, input);
    } else if (display == Display::TABLE) {
        return tableLayout(tree, box, input);
    } else if (display == Display::INTERNAL) {
        return Output{};
    } else {
        return blockLayout(tree, box, input);
    }
}

InsetsPx computeMargins(Tree &tree, Box &box, Input input) {
    InsetsPx res;
    auto margin = box.style->margin;

    res.top = resolve(tree, box, margin->top, input.containingBlock.height);
    res.end = resolve(tree, box, margin->end, input.containingBlock.width);
    res.bottom = resolve(tree, box, margin->bottom, input.containingBlock.height);
    res.start = resolve(tree, box, margin->start, input.containingBlock.width);

    return res;
}

InsetsPx computeBorders(Tree &tree, Box &box) {
    InsetsPx res;
    auto borders = box.style->borders;

    if (borders->top.style != Gfx::BorderStyle::NONE)
        res.top = resolve(tree, box, borders->top.width);

    if (borders->end.style != Gfx::BorderStyle::NONE)
        res.end = resolve(tree, box, borders->end.width);

    if (borders->bottom.style != Gfx::BorderStyle::NONE)
        res.bottom = resolve(tree, box, borders->bottom.width);

    if (borders->start.style != Gfx::BorderStyle::NONE)
        res.start = resolve(tree, box, borders->start.width);

    return res;
}

static InsetsPx _computePaddings(Tree &tree, Box &box, Vec2Px containingBlock) {
    InsetsPx res;
    auto padding = box.style->padding;

    res.top = resolve(tree, box, padding->top, containingBlock.height);
    res.end = resolve(tree, box, padding->end, containingBlock.width);
    res.bottom = resolve(tree, box, padding->bottom, containingBlock.height);
    res.start = resolve(tree, box, padding->start, containingBlock.width);

    return res;
}

static Math::Radii<Px> _computeRadii(Tree &tree, Box &box, Vec2Px size) {
    auto radii = box.style->borders->radii;
    Math::Radii<Px> res;

    res.a = resolve(tree, box, radii.a, size.height);
    res.b = resolve(tree, box, radii.b, size.width);
    res.c = resolve(tree, box, radii.c, size.width);
    res.d = resolve(tree, box, radii.d, size.height);
    res.e = resolve(tree, box, radii.e, size.height);
    res.f = resolve(tree, box, radii.f, size.width);
    res.g = resolve(tree, box, radii.g, size.width);
    res.h = resolve(tree, box, radii.h, size.height);

    return res;
}

Vec2Px computeIntrinsicSize(Tree &tree, Box &box, IntrinsicSize intrinsic, Vec2Px containingBlock) {
    if (intrinsic == IntrinsicSize::AUTO) {
        panic("bad argument");
    }

    auto borders = computeBorders(tree, box);
    auto padding = _computePaddings(tree, box, containingBlock);

    auto [size] = _contentLayout(
        tree,
        box,
        {
            .commit = Commit::NO,
            .intrinsic = intrinsic,
            .knownSize = {NONE, NONE},
        }
    );

    return size + padding.all() + borders.all();
}

static Opt<Px> _computeSpecifiedSize(Tree &tree, Box &box, Size size, Vec2Px containingBlock, bool isWidth) {
    if (size == Size::MIN_CONTENT) {
        auto intrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MIN_CONTENT, containingBlock);
        return isWidth ? Opt<Px>{intrinsicSize.x} : Opt<Px>{NONE};
    } else if (size == Size::MAX_CONTENT) {
        auto intrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MAX_CONTENT, containingBlock);
        return isWidth ? Opt<Px>{intrinsicSize.x} : Opt<Px>{NONE};
    } else if (size == Size::FIT_CONTENT) {
        auto minIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MIN_CONTENT, containingBlock);
        auto maxIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MAX_CONTENT, containingBlock);
        auto stretchIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::STRETCH_TO_FIT, containingBlock);

        if (isWidth)
            return clamp(stretchIntrinsicSize.x, minIntrinsicSize.x, maxIntrinsicSize.x);
        else
            return clamp(stretchIntrinsicSize.y, minIntrinsicSize.y, maxIntrinsicSize.y);
    } else if (size == Size::AUTO) {
        return NONE;
    } else if (size == Size::LENGTH) {
        return resolve(tree, box, size.value, isWidth ? containingBlock.x : containingBlock.y);
    } else {
        logWarn("unknown specified size: {}", size);
        return 0_px;
    }
}

Output layout(Tree &tree, Box &box, Input input) {
    // FIXME: confirm how the preffered width/height parameters interacts with intrinsic size argument from input
    auto borders = computeBorders(tree, box);
    auto padding = _computePaddings(tree, box, input.containingBlock);
    auto sizing = box.style->sizing;

    if (input.knownSize.width == NONE) {
        auto specifiedWidth = _computeSpecifiedSize(tree, box, sizing->width, input.containingBlock, true);
        input.knownSize.width = specifiedWidth;
    }

    input.knownSize.width = input.knownSize.width.map([&](auto s) {
        return max(0_px, s - padding.horizontal() - borders.horizontal());
    });

    if (input.knownSize.height == NONE) {
        auto specifiedHeight = _computeSpecifiedSize(tree, box, sizing->height, input.containingBlock, false);
        input.knownSize.height = specifiedHeight;
    }

    input.knownSize.height = input.knownSize.height.map([&](auto s) {
        return max(0_px, s - padding.vertical() - borders.vertical());
    });

    input.position = input.position + borders.topStart() + padding.topStart();

    auto [size] = _contentLayout(tree, box, input);

    size.width = input.knownSize.width.unwrapOr(size.width);
    size.height = input.knownSize.height.unwrapOr(size.height);

    size = size + padding.all() + borders.all();

    if (input.commit == Commit::YES) {
        box.layout.position = input.position - borders.topStart() - padding.topStart();
        box.layout.borderSize = size;
        box.layout.padding = padding;
        box.layout.borders = borders;
        box.layout.radii = _computeRadii(tree, box, size);
    }

    return Output::fromSize(size);
}

} // namespace Vaev::Layout
