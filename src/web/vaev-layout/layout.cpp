#include "layout.h"

#include "block.h"
#include "flex.h"
#include "grid.h"
#include "inline.h"
#include "positioned.h"
#include "replaced.h"
#include "table.h"
#include "values.h"

namespace Vaev::Layout {

static Opt<Rc<FormatingContext>> _constructFormatingContext(Box& box) {
    auto display = box.style->display;

    if (box.content.is<Karm::Image::Picture>()) {
        return constructReplacedFormatingContext(box);
    } else if (box.content.is<Rc<Text::Prose>>()) {
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
    if (box.formatingContext == NONE)
        box.formatingContext = _constructFormatingContext(box);
    if (not box.formatingContext)
        return Output{};
    return box.formatingContext->unwrap().run(tree, box, input, startAt, stopAt);
}

InsetsPx computeMargins(Tree& tree, Box& box, Input input) {
    InsetsPx res;
    auto margin = box.style->margin;

    res.top = resolve(tree, box, margin->top, input.containingBlock.height);
    res.end = resolve(tree, box, margin->end, input.containingBlock.width);
    res.bottom = resolve(tree, box, margin->bottom, input.containingBlock.height);
    res.start = resolve(tree, box, margin->start, input.containingBlock.width);

    return res;
}

InsetsPx computeBorders(Tree& tree, Box& box) {
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

static InsetsPx _computePaddings(Tree& tree, Box& box, Vec2Px containingBlock) {
    InsetsPx res;
    auto padding = box.style->padding;

    res.top = resolve(tree, box, padding->top, containingBlock.height);
    res.end = resolve(tree, box, padding->end, containingBlock.width);
    res.bottom = resolve(tree, box, padding->bottom, containingBlock.height);
    res.start = resolve(tree, box, padding->start, containingBlock.width);

    return res;
}

static Math::Radii<Px> _computeRadii(Tree& tree, Box& box, Vec2Px size) {
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

Vec2Px computeIntrinsicSize(Tree& tree, Box& box, IntrinsicSize intrinsic, Vec2Px containingBlock) {
    if (intrinsic == IntrinsicSize::AUTO) {
        panic("bad argument");
    }

    auto borders = computeBorders(tree, box);
    auto padding = _computePaddings(tree, box, containingBlock);

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

static Opt<Px> _computeSpecifiedSize(Tree& tree, Box& box, Size size, Vec2Px containingBlock, bool isWidth) {
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

static Res<None, Output> _shouldAbortFragmentingBeforeLayout(Fragmentainer& fc, Input input) {
    if (not fc.acceptsFit(
            input.position.y,
            0_px,
            input.pendingVerticalSizes
        ))
        return Output{
            .size = Vec2Px{0_px, 0_px},
            .completelyLaidOut = false,
            .breakpoint = Breakpoint::buildOverflow()
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
        .advanceCase = Breakpoint::ADVANCE_CASE::ADVANCE_WITHOUT_CHILDREN
    };

    outputBreakpoint = bottomOfContentBreakForTopMonolitic;
}

Output layout(Tree& tree, Box& box, Input input) {

    // FIXME: confirm how the preferred width/height parameters interacts with intrinsic size argument from input
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

    input.availableSpace.height = max(0_px, input.availableSpace.height - padding.vertical() - borders.vertical());
    input.availableSpace.width = max(0_px, input.availableSpace.width - padding.horizontal() - borders.horizontal());

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

        out.size = size + padding.all() + borders.all();

        if (isMonolithicDisplay)
            tree.fc.leaveMonolithicBox();

        return out;
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
            currFrag.metrics.radii = _computeRadii(tree, box, size);

            parentFrag->add(std::move(currFrag));
        }

        return Output{
            .size = size,
            .completelyLaidOut = out.completelyLaidOut
        };
    }
}

Output layout(Tree& tree, Input input) {
    auto out = layout(tree, tree.root, input);
    if (input.fragment)
        layoutPositioned(tree, input.fragment->children[0], input.containingBlock);
    return out;
}

Tuple<Output, Frag> layoutCreateFragment(Tree& tree, Input input) {
    auto root = Layout::Frag();
    input.fragment = &root;
    auto out = layout(tree, input);
    return {out, std::move(root.children[0])};
}

} // namespace Vaev::Layout
