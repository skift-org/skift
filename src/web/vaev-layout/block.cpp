#include "input_output.h"
#include "layout.h"
#include "tree.h"

namespace Vaev::Layout {

void maybeProcessChildBreakpoint(FragmentationContext &fc, Breakpoint &currentBreakpoint, usize childIndex, bool currBoxIsBreakAvoid, Opt<Breakpoint> maybeChildBreakpoint) {
    if (not fc.isDiscoveryMode())
        return;

    // if we are in a monolitic context, we might not have breakpoints
    if (not maybeChildBreakpoint)
        return;

    // breakpoint inside child (from this blocks perspective)
    // BREAK CLASS X (recursive case)
    currentBreakpoint.overrideIfBetter(
        Breakpoint::buildFromChild(
            std::move(maybeChildBreakpoint.unwrap()),
            childIndex + 1,
            currBoxIsBreakAvoid
        )
    );
}

Res<None, Output> processBreakpointsAfterChild(FragmentationContext &fc, Breakpoint &currentBreakpoint, Box &parentBox, usize childIndex, Vec2Px currentBoxSize, bool childCompletelyLaidOut) {
    if (not fc.isDiscoveryMode())
        return Ok(NONE);

    // last child was not completly laid out, we need to abort with our best breakpoint
    if (not childCompletelyLaidOut) {
        return Output{
            .size = currentBoxSize,
            .completelyLaidOut = false,
            .breakpoint = currentBreakpoint
        };
    }

    Box &childBox = parentBox.children()[childIndex];
    bool isLastChild = childIndex + 1 == parentBox.children().len();

    // breakpoint right after child
    // this can only happen IF child was fully laid out and its not last child (not class C)
    // BREAK CLASS A
    if (not isLastChild) {
        bool breakIsAvoided =
            parentBox.style->break_->inside == BreakInside::AVOID or
            childBox.style->break_->after == BreakBetween::AVOID or
            (not isLastChild and parentBox.children()[childIndex + 1].style->break_->before == BreakBetween::AVOID);

        currentBreakpoint.overrideIfBetter(
            Breakpoint::buildClassB(
                childIndex + 1,
                breakIsAvoided
            )
        );
    }

    // FORCED BREAK
    if (childBox.style->break_->after == BreakBetween::PAGE) {
        return Output{
            .size = currentBoxSize,
            .completelyLaidOut = false,
            .breakpoint = Breakpoint::buildForced(
                childIndex + 1
            )
        };
    }

    return Ok(NONE);
}

Res<None, Output> processBreakpointsBeforeChild(usize endAt, Vec2Px currentSize, bool forcedBreakBefore, usize startAt) {
    // FORCED BREAK
    if (forcedBreakBefore and not(startAt == endAt)) {
        return Output{
            .size = currentSize,
            .completelyLaidOut = false,
            .breakpoint = Breakpoint::buildForced(endAt)
        };
    }

    return Ok(NONE);
}

Output fragmentEmptyBox(Tree &tree, Input input) {
    // put this here instead of in layout.py since we want to know if its the empty box case
    Vec2Px knownSize{input.knownSize.x.unwrapOr(0_px), input.knownSize.y.unwrapOr(0_px)};
    if (tree.fc.isDiscoveryMode()) {
        if (tree.fc.acceptsFit(
                input.position.y,
                knownSize.y,
                input.pendingVerticalSizes
            )) {
            return Output{
                .size = knownSize,
                .completelyLaidOut = true,
            };
        } else {
            return Output{
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::buildOverflow()
            };
        }
    } else {
        // FIXME: we should be breaking empty boxes using pixels or percentages, this behaviour is not compliant
        Px verticalSpaceLeft = tree.fc.leftVerticalSpace(
            input.position.y, input.pendingVerticalSizes
        );
        return Output{
            .size = {knownSize.x, min(knownSize.y, verticalSpaceLeft)},
            .completelyLaidOut = verticalSpaceLeft >= knownSize.y,
        };
    }
}

// https://www.w3.org/TR/CSS22/visuren.html#normal-flow
struct BlockFormatingContext {

    Px computeCAPMIN(Tree &tree, Box &box, Input input, Px inlineSize) {
        Px capmin{};
        for (auto &c : box.children()) {
            if (c.style->display != Display::TABLE_BOX) {
                auto margin = computeMargins(
                    tree, c,
                    {
                        .containingBlock = {inlineSize, input.knownSize.y.unwrapOr(0_px)},
                    }
                );

                auto minContentContrib = computeIntrinsicSize(
                    tree, c, IntrinsicSize::MIN_CONTENT, input.containingBlock
                );

                capmin = max(
                    capmin,
                    minContentContrib.width + margin.horizontal()
                );
            }
        }

        return capmin;
    }

    Output run(Tree &tree, Box &box, Input input, usize startAt, Opt<usize> stopAt) {
        Px blockSize = 0_px;
        Px inlineSize = input.knownSize.width.unwrapOr(0_px);

        if (box.children().len() == 0) {
            return fragmentEmptyBox(tree, input);
        }

        // NOTE: Our parent has no clue about our width but wants us to commit,
        //       we need to compute it first
        if (input.fragment and not input.knownSize.width)
            inlineSize = run(tree, box, input.withFragment(nullptr), startAt, stopAt).width();

        Breakpoint currentBreakpoint;

        usize endChildren = stopAt.unwrapOr(box.children().len());

        bool blockWasCompletelyLaidOut = false;

        for (usize i = startAt; i < endChildren; ++i) {
            auto &c = box.children()[i];

            try$(
                processBreakpointsBeforeChild(
                    i,
                    Vec2Px{inlineSize, blockSize},
                    c.style->break_->before == BreakBetween::PAGE,
                    startAt
                )
            );

            // TODO: Implement floating
            // if (c.style->float_ != Float::NONE)
            //     continue;

            Input childInput = {
                .fragment = input.fragment,
                .intrinsic = input.intrinsic,
                .availableSpace = {input.availableSpace.x, 0_px},
                .containingBlock = {inlineSize, input.knownSize.y.unwrapOr(0_px)},
                .breakpointTraverser = input.breakpointTraverser.traverseInsideUsingIthChild(i),
                .pendingVerticalSizes = input.pendingVerticalSizes,
            };

            auto margin = computeMargins(tree, c, childInput);

            Opt<Px> childInlineSize = NONE;
            if (c.style->sizing->width == Size::AUTO) {
                childInlineSize = inlineSize - margin.horizontal();
            }

            if (c.style->position != Position::ABSOLUTE) {
                blockSize += margin.top;
                if (input.fragment or input.knownSize.x)
                    childInput.knownSize.width = childInlineSize;
            }

            childInput.position = input.position + Vec2Px{margin.start, blockSize};

            if (c.style->display == Display::Internal::TABLE_BOX) {
                childInput.capmin = computeCAPMIN(tree, box, input, inlineSize);
            }

            auto output = layout(
                tree,
                c,
                childInput
            );
            if (c.style->position != Position::ABSOLUTE) {
                blockSize += output.size.y + margin.bottom;
            }

            maybeProcessChildBreakpoint(
                tree.fc,
                currentBreakpoint,
                i,
                box.style->break_->inside == BreakInside::AVOID,
                output.breakpoint
            );

            try$(processBreakpointsAfterChild(
                tree.fc,
                currentBreakpoint,
                box,
                i,
                Vec2Px{inlineSize, blockSize},
                output.completelyLaidOut
            ));

            if (tree.fc.allowBreak() and i + 1 == endChildren) {
                blockWasCompletelyLaidOut = output.completelyLaidOut and i + 1 == box.children().len();
            }

            inlineSize = max(inlineSize, output.size.x + margin.horizontal());
        }

        return Output{
            .size = Vec2Px{inlineSize, blockSize},
            .completelyLaidOut = blockWasCompletelyLaidOut,
            .breakpoint = currentBreakpoint
        };
    }
};

Output blockLayout(Tree &tree, Box &box, Input input, usize startAt, Opt<usize> stopAt) {
    BlockFormatingContext fc;
    return fc.run(tree, box, input, startAt, stopAt);
}

} // namespace Vaev::Layout
