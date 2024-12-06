#include "block.h"

#include "box.h"
#include "layout.h"

namespace Vaev::Layout {

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

    Output run(Tree &tree, Box &box, Input input) {

        Px blockSize = 0_px;
        Px inlineSize = input.knownSize.width.unwrapOr(0_px);

        // NOTE: Our parent has no clue about our width but wants us to commit,
        //       we need to compute it first
        if (input.commit == Commit::YES and not input.knownSize.width)
            inlineSize = run(tree, box, input.withCommit(Commit::NO)).width();

        for (auto &c : box.children()) {
            // TODO: Implement floating
            // if (c.style->float_ != Float::NONE)
            //     continue;

            Input childInput = {
                .commit = input.commit,
                .intrinsic = input.intrinsic,
                .availableSpace = {input.availableSpace.x, 0_px},
                .containingBlock = {inlineSize, input.knownSize.y.unwrapOr(0_px)},
            };

            auto margin = computeMargins(tree, c, childInput);

            Opt<Px> childInlineSize = NONE;
            if (c.style->sizing->width == Size::AUTO) {
                childInlineSize = inlineSize - margin.horizontal();
            }

            if (c.style->position != Position::ABSOLUTE) {
                blockSize += margin.top;
                if (input.commit == Commit::YES or input.knownSize.x)
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

            inlineSize = max(inlineSize, output.size.x + margin.horizontal());
        }

        return Output::fromSize({
            inlineSize,
            blockSize,
        });
    }
};

Output blockLayout(Tree &tree, Box &box, Input input) {
    BlockFormatingContext fc;
    return fc.run(tree, box, input);
}

} // namespace Vaev::Layout
