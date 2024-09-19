#include "block.h"

#include "frag.h"
#include "layout.h"

namespace Vaev::Layout {

// https://www.w3.org/TR/CSS22/visuren.html#normal-flow
struct BlockFormatingContext {
    static Px _determineWidth(Tree &t, Frag &f, Input input) {
        Px width = Px{0};
        for (auto &c : f.children()) {
            auto ouput = layout(
                t,
                c,
                {
                    .commit = Commit::NO,
                    .intrinsic = input.intrinsic,
                }
            );

            width = max(width, ouput.size.x);
        }

        return width;
    }

    Output run(Tree &t, Frag &f, Input input) {
        Px blockSize = Px{0};
        Px inlineSize = input.knownSize.width.unwrapOrElse([&] {
            return _determineWidth(t, f, input);
        });

        for (auto &c : f.children()) {
            if (c.style->float_ != Float::NONE)
                continue;

            Opt<Px> childInlineSize = NONE;
            if (c.style->sizing->width == Size::AUTO and
                c.style->display != Display::TABLE) {
                childInlineSize = inlineSize;
            }

            Input childInput = {
                .commit = input.commit,
                .availableSpace = {inlineSize, Px{0}},
                .containingBlock = {inlineSize, Px{0}},
            };

            auto margin = computeMargins(t, c, childInput);

            if (c.style->position != Position::ABSOLUTE) {
                blockSize += margin.top;
                childInput.knownSize.width = childInlineSize;
            }

            childInput.position = input.position + Vec2Px{margin.start, blockSize};

            auto ouput = layout(
                t,
                c,
                childInput
            );

            if (c.style->position != Position::ABSOLUTE) {
                blockSize += ouput.size.y + margin.bottom;
            }
        }

        // layoutFloat(t, f, input.containingBlock);

        return Output::fromSize({
            inlineSize,
            blockSize,
        });
    }
};

Output blockLayout(Tree &t, Frag &f, Input input) {
    BlockFormatingContext fc;
    return fc.run(t, f, input);
}

} // namespace Vaev::Layout
