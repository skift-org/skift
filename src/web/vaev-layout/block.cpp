#include "block.h"

#include "box.h"
#include "layout.h"

namespace Vaev::Layout {

// https://www.w3.org/TR/CSS22/visuren.html#normal-flow
struct BlockFormatingContext {
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
                .availableSpace = {inlineSize, 0_px},
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

            auto ouput = layout(
                tree,
                c,
                childInput
            );

            if (c.style->position != Position::ABSOLUTE) {
                blockSize += ouput.size.y + margin.bottom;
            }

            inlineSize = max(inlineSize, ouput.size.x + margin.start + margin.end);
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
