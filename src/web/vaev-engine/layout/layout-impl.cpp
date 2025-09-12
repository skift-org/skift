module;

#include <karm-gfx/borders.h>
#include <karm-gfx/prose.h>
#include <karm-logger/logger.h>

module Vaev.Engine;

import Karm.Image;
import :values;
import :layout.block;
import :layout.flex;
import :layout.grid;
import :layout.inline_;
import :layout.replaced;
import :layout.positioned;
import :layout.table;
import :layout.values;

namespace Vaev::Layout {

static Opt<Rc<FormatingContext>> _constructFormatingContext(Box& box) {
    auto display = box.style->display;

    if (box.isReplaced()) {
        return constructReplacedFormatingContext(box);
    } else if (box.content.is<InlineBox>()) {
        return constructInlineFormatingContext(box);
    } else if (
        display == Display::FLOW or
        display == Display::FLOW_ROOT or
        display == Display::TABLE_CELL or
        display == Display::TABLE_CAPTION or
        display == Display::TABLE
    ) {
        return constructBlockFormatingContext(box);
    } else if (display == Display::FLEX) {
        return constructFlexFormatingContext(box);
    } else if (display == Display::GRID) {
        return constructGridFormatingContext(box);
    } else if (display == Display::TABLE_BOX) {
        return constructTableFormatingContext(box);
    } else if (display == Display::INTERNAL) {
        return NONE;
    } else {
        return constructBlockFormatingContext(box);
    }
}

Output _contentLayout(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) {
    if (box.formatingContext == NONE) {
        box.formatingContext = _constructFormatingContext(box);
        if (box.formatingContext)
            box.formatingContext.unwrap()->build(tree, box);
    }
    if (not box.formatingContext)
        return Output{};
    return box.formatingContext.unwrap()->run(tree, box, input, startAt, stopAt);
}

InsetsAu computeMargins(Tree& tree, Box& box, Input input) {
    // Boxes that make up a table do not have margins.
    if (box.style->display.isTableInternal())
        return {};

    InsetsAu res;
    auto margin = box.style->margin;

    res.top = resolve(tree, box, margin->top, input.containingBlock.height);
    res.end = resolve(tree, box, margin->end, input.containingBlock.width);
    res.bottom = resolve(tree, box, margin->bottom, input.containingBlock.height);
    res.start = resolve(tree, box, margin->start, input.containingBlock.width);

    return res;
}

// https://www.w3.org/TR/css-values-4/#snap-a-length-as-a-border-width
Au _snapLengthAsBorderWidth(Au v) {
    if (v < 1_au)
        return ceil(v);
    return floor(v);
}

InsetsAu computeBorders(Tree& tree, Box& box) {
    InsetsAu res;
    auto borders = box.style->borders;

    if (borders->top.style != Gfx::BorderStyle::NONE)
        res.top = _snapLengthAsBorderWidth(resolve(tree, box, borders->top.width));

    if (borders->end.style != Gfx::BorderStyle::NONE)
        res.end = _snapLengthAsBorderWidth(resolve(tree, box, borders->end.width));

    if (borders->bottom.style != Gfx::BorderStyle::NONE)
        res.bottom = _snapLengthAsBorderWidth(resolve(tree, box, borders->bottom.width));

    if (borders->start.style != Gfx::BorderStyle::NONE)
        res.start = _snapLengthAsBorderWidth(resolve(tree, box, borders->start.width));

    return res;
}

InsetsAu computePaddings(Tree& tree, Box& box, Vec2Au containingBlock) {
    // In a table only table cell have padding
    if (box.style->display.isTableInternal() and box.style->display != Display::TABLE_CELL)
        return {};

    InsetsAu res;
    auto padding = box.style->padding;

    res.top = resolve(tree, box, padding->top, containingBlock.width);
    res.end = resolve(tree, box, padding->end, containingBlock.width);
    res.bottom = resolve(tree, box, padding->bottom, containingBlock.width);
    res.start = resolve(tree, box, padding->start, containingBlock.width);

    return res;
}

Math::Radii<Au> computeRadii(Tree& tree, Box& box, Vec2Au size) {
    auto radii = box.style->borders->radii;
    Math::Radii<Au> res;

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

Vec2Au computeIntrinsicSize(Tree& tree, Box& box, IntrinsicSize intrinsic, Vec2Au containingBlock) {
    if (intrinsic == IntrinsicSize::AUTO) {
        panic("bad argument");
    }

    auto borders = computeBorders(tree, box);
    auto padding = computePaddings(tree, box, containingBlock);

    auto output = _contentLayout(
        tree,
        box,
        {
            .intrinsic = intrinsic,
            .knownSize = {NONE, NONE},
        },
        0, NONE
    );

    return output.size + padding.all() + borders.all();
}

Opt<Au> computeSpecifiedSize(Tree& tree, Box& box, Size size, Vec2Au containingBlock, bool isWidth) {
    if (size.is<Keywords::MinContent>()) {
        auto intrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MIN_CONTENT, containingBlock);
        return isWidth ? Opt<Au>{intrinsicSize.x} : Opt<Au>{NONE};
    } else if (size.is<Keywords::MaxContent>()) {
        auto intrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MAX_CONTENT, containingBlock);
        return isWidth ? Opt<Au>{intrinsicSize.x} : Opt<Au>{NONE};
    } else if (size.is<FitContent>()) {
        auto minIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MIN_CONTENT, containingBlock);
        auto maxIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MAX_CONTENT, containingBlock);
        auto stretchIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::STRETCH_TO_FIT, containingBlock);

        if (isWidth)
            return clamp(stretchIntrinsicSize.x, minIntrinsicSize.x, maxIntrinsicSize.x);
        else
            return clamp(stretchIntrinsicSize.y, minIntrinsicSize.y, maxIntrinsicSize.y);
    } else if (size.is<Keywords::Auto>()) {
        return NONE;
    } else if (auto calc = size.is<CalcValue<PercentOr<Length>>>()) {
        return resolve(tree, box, *calc, isWidth ? containingBlock.x : containingBlock.y);
    } else {
        logWarn("unknown specified size: {}", size);
        return 0_au;
    }
}

static Res<None, Output> _shouldAbortFragmentingBeforeLayout(Fragmentainer& fc, Input input) {
    if (not fc.acceptsFit(
            input.position.y,
            0_au,
            input.pendingVerticalSizes
        ))
        return Output{
            .size = Vec2Au{0_au, 0_au},
            .completelyLaidOut = false,
            .breakpoint = Breakpoint::overflow()
        };

    return Ok(NONE);
}

static void _maybeSetMonolithicBreakpoint(Fragmentainer& fc, bool isMonolticDisplay, bool childCompletelyLaidOut, usize boxChildrenLen, Opt<Breakpoint>& outputBreakpoint) {
    if (not fc.isMonolithicBox() or
        not isMonolticDisplay or
        fc.hasInfiniteDimensions())
        return;

    if (not childCompletelyLaidOut)
        panic("monolitic blocks should always be completly laid out");

    // NOTE: wont abstract this since this is currently a workaround since we dont have fragmentation for table,flex
    Breakpoint bottomOfContentBreakForTopMonolitic{
        .endIdx = boxChildrenLen,
        .appeal = Breakpoint::Appeal::CLASS_B,
        .advance = Breakpoint::Advance::WITHOUT_CHILDREN
    };

    outputBreakpoint = bottomOfContentBreakForTopMonolitic;
}

void fillKnownSizeWithSpecifiedSizeIfEmpty(Tree& tree, Box& box, Input& input) {
    auto sizing = box.style->sizing;

    if (input.knownSize.width == NONE) {
        auto specifiedWidth = computeSpecifiedSize(tree, box, sizing->width, input.containingBlock, true);
        input.knownSize.width = specifiedWidth;
    }

    if (input.knownSize.height == NONE) {
        auto specifiedHeight = computeSpecifiedSize(tree, box, sizing->height, input.containingBlock, false);
        input.knownSize.height = specifiedHeight;
    }
}

Output layout(Tree& tree, Box& box, Input input) {
    // FIXME: confirm how the preferred width/height parameters interacts with intrinsic size argument from input

    fillKnownSizeWithSpecifiedSizeIfEmpty(tree, box, input);

    auto borders = computeBorders(tree, box);
    auto padding = computePaddings(tree, box, input.containingBlock);

    input.knownSize.width = input.knownSize.width.map([&](auto s) {
        return max(0_au, s - padding.horizontal() - borders.horizontal());
    });

    input.knownSize.height = input.knownSize.height.map([&](auto s) {
        return max(0_au, s - padding.vertical() - borders.vertical());
    });

    input.availableSpace.height = max(0_au, input.availableSpace.height - padding.vertical() - borders.vertical());
    input.availableSpace.width = max(0_au, input.availableSpace.width - padding.horizontal() - borders.horizontal());

    input.position = input.position + borders.topStart() + padding.topStart();
    input.pendingVerticalSizes += borders.bottom + padding.bottom;

    bool isMonolithicDisplay =
        box.style->display == Display::Inside::FLEX or
        box.style->display == Display::Inside::GRID;

    usize startAt = tree.fc.allowBreak() ? input.breakpointTraverser.getStart().unwrapOr(0) : 0;

    if (tree.fc.isDiscoveryMode()) {
        try$(_shouldAbortFragmentingBeforeLayout(tree.fc, input));

        if (isMonolithicDisplay)
            tree.fc.enterMonolithicBox();

        // TODO: Class C breakpoint

        auto out = _contentLayout(tree, box, input, startAt, NONE);

        // NOTE: assert since algo is still a bit experimental
        if (not out.completelyLaidOut and out.breakpoint == NONE)
            panic("if it was not completely laid out, there should be a breakpoint");

        auto size = out.size;
        size.width = input.knownSize.width.unwrapOr(size.width);
        if (out.completelyLaidOut and not input.breakpointTraverser.prevIteration) {
            size.height = input.knownSize.height.unwrapOr(size.height);
        }

        // TODO: Class C breakpoint

        _maybeSetMonolithicBreakpoint(
            tree.fc,
            isMonolithicDisplay,
            out.completelyLaidOut,
            box.children().len(),
            out.breakpoint
        );

        if (isMonolithicDisplay)
            tree.fc.leaveMonolithicBox();

        return {
            .size = size + padding.all() + borders.all(),
            .completelyLaidOut = out.completelyLaidOut,
            .breakpoint = out.breakpoint,
            .firstBaselineSet = out.firstBaselineSet.translate(padding.top + borders.top),
            .lastBaselineSet = out.lastBaselineSet.translate(padding.top + borders.top),
        };
    } else {
        Opt<usize> stopAt = tree.fc.allowBreak()
                                ? input.breakpointTraverser.getEnd()
                                : NONE;

        auto parentFrag = input.fragment;
        Frag currFrag(&box);
        input.fragment = input.fragment ? &currFrag : nullptr;

        if (isMonolithicDisplay)
            tree.fc.enterMonolithicBox();

        auto out = _contentLayout(tree, box, input, startAt, stopAt);

        auto size = out.size;
        size.width = input.knownSize.width.unwrapOr(size.width);
        if ((not tree.fc.allowBreak()) or (out.completelyLaidOut and not input.breakpointTraverser.prevIteration)) {
            size.height = input.knownSize.height.unwrapOr(size.height);
        }

        if (isMonolithicDisplay)
            tree.fc.leaveMonolithicBox();

        size = size + padding.all() + borders.all();

        if (parentFrag) {
            currFrag.metrics.position = input.position - borders.topStart() - padding.topStart();
            currFrag.metrics.borderSize = size;
            currFrag.metrics.padding = padding;
            currFrag.metrics.borders = borders;
            currFrag.metrics.radii = computeRadii(tree, box, size);
            currFrag.metrics.outlineOffset = resolve(tree, box, box.style->outline->offset);
            currFrag.metrics.outlineWidth = resolve(tree, box, box.style->outline->width);

            parentFrag->add(std::move(currFrag));
        }

        return Output{
            .size = size,
            .completelyLaidOut = out.completelyLaidOut,
            .firstBaselineSet = out.firstBaselineSet.translate(padding.top + borders.top),
            .lastBaselineSet = out.lastBaselineSet.translate(padding.top + borders.top),
        };
    }
}

Output layout(Tree& tree, Input input) {
    auto out = layout(tree, tree.root, input);
    if (input.fragment)
        layoutPositioned(tree, input.fragment->children()[0], input.containingBlock);
    return out;
}

Tuple<Output, Frag> layoutCreateFragment(Tree& tree, Input input) {
    auto root = Layout::Frag();
    input.fragment = &root;
    auto out = layout(tree, input);
    return {out, std::move(root.children()[0])};
}

} // namespace Vaev::Layout
