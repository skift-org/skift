module;

#include <karm-text/prose.h>
#include <vaev-base/display.h>
#include <vaev-base/keywords.h>

export module Vaev.Layout:inline_;

import :base;
import :layout;

namespace Vaev::Layout {

struct InlineFormatingContext : FormatingContext {

    // https://www.w3.org/TR/css-inline-3/#baseline-source
    BaselinePositionsSet getUsedBaselineFromBox(Box const& childBox, Output const& output) {
        if (childBox.style->baseline->source == Keywords::FIRST)
            return output.firstBaselineSet;
        if (childBox.style->baseline->source == Keywords::LAST)
            return output.lastBaselineSet;

        if (childBox.style->display == Display::INLINE and childBox.style->display == Display::FLOW_ROOT)
            return output.lastBaselineSet;
        return output.firstBaselineSet;
    }

    BaselinePositionsSet _computeBaselinePositions(InlineBox& inlineBox, Au baselinePosition) {
        auto baselineSet = inlineBox.prose->_style.font.baselineSet();

        return BaselinePositionsSet{
            .alphabetic = Au{baselineSet.alphabetic} + baselinePosition,
            .xHeight = Au{baselineSet.xHeight} + baselinePosition,
            .xMiddle = Au{baselineSet.xMiddle} + baselinePosition,
            .capHeight = Au{baselineSet.capHeight} + baselinePosition,
        };
    }

    virtual Output run([[maybe_unused]] Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        // NOTE: We are not supposed to get there if the content is not a prose
        auto& inlineBox = box.content.unwrap<InlineBox>("inlineLayout");

        auto inlineSize = input.knownSize.x.unwrapOrElse([&] {
            if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
                return 0_au;
            } else if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
                return Limits<Au>::MAX;
            } else {
                return input.availableSpace.x;
            }
        });

        auto& prose = inlineBox.prose;

        for (auto strutCell : prose->cellsWithStruts()) {
            auto& boxStrutCell = *strutCell->strut();

            auto& atomicBox = *inlineBox.atomicBoxes[boxStrutCell.id];

            // NOTE: We set the same availableSpace to child inline boxes since line wrapping is possible i.e. in the
            // worst case, they will take up the whole availableSpace, and a line break will be done right before them
            auto atomicBoxOutput = layout(
                tree,
                atomicBox,
                Input{
                    .knownSize = {NONE, NONE},
                    .availableSpace = {inlineSize, input.availableSpace.y},
                }
            );

            boxStrutCell.size = atomicBoxOutput.size;
            // FIXME: hard-coding alphabetic alignment, missing alignment-baseline and dominant-baseline
            boxStrutCell.baseline = getUsedBaselineFromBox(atomicBox, atomicBoxOutput).alphabetic;
        }

        // FIXME: prose has a ongoing state that is not reset between layout calls, but it should be
        prose->_blocksMeasured = false;
        auto size = prose->layout(inlineSize);

        auto firstBaselineSet = _computeBaselinePositions(inlineBox, first(prose->_lines).baseline);
        auto lastBaselineSet = _computeBaselinePositions(inlineBox, last(prose->_lines).baseline);

        for (auto strutCell : prose->cellsWithStruts()) {
            auto runeIdx = strutCell->runeRange.start;
            auto positionInProse = prose->queryPosition(runeIdx);

            auto& boxStrutCell = *strutCell->strut();

            Math::Vec2<Opt<Au>> knownSize = {
                boxStrutCell.size.x,
                boxStrutCell.size.y
            };

            layout(
                tree,
                inlineBox.atomicBoxes[boxStrutCell.id],
                Input{
                    .fragment = input.fragment,
                    .knownSize = knownSize,
                    .position = input.position + positionInProse,
                }
            );
        }

        if (tree.fc.allowBreak() and not tree.fc.acceptsFit(
                                         input.position.y,
                                         size.y,
                                         input.pendingVerticalSizes
                                     )) {
            return {
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::overflow()
            };
        }

        return {
            .size = size,
            .completelyLaidOut = true,
            .firstBaselineSet = firstBaselineSet,
            .lastBaselineSet = lastBaselineSet,
        };
    }
};

export Rc<FormatingContext> constructInlineFormatingContext(Box&) {
    return makeRc<InlineFormatingContext>();
}

} // namespace Vaev::Layout
