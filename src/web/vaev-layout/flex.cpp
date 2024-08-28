#include "flex.h"

#include "frag.h"

namespace Vaev::Layout {

struct FlexItem {
    Frag &frag;
};

struct FlexLine {
    Slice<FlexItem> items;
};

Output flexLayout(Tree &t, Frag &f, Input input) {
    // https://www.w3.org/TR/css-flexbox-1/#layout-algorithm

    // 1. Generate anonymous flex items
    Vec<FlexItem> flexItems;

    // TODO: Implement this step

    // 2. Determine the available main and cross space for the flex items.
    // TODO: Implement this step

    // 3. Determine the flex base size and hypothetical main size of each item
    // TODO: Implement this step

    // 4. Determine the main size of the flex container
    // TODO: Implement this step

    // 5. Collect flex items into flex lines
    Vec<FlexLine> flexLines;

    // TODO: Implement this step

    // 6. Resolve the flexible lengths
    // TODO: Implement this step

    // 7. Determine the hypothetical cross size of each item
    // TODO: Implement this step

    // 8. Calculate the cross size of each flex line
    // TODO: Implement this step

    // 9. Handle 'align-content: stretch'.
    // TODO: Implement this step

    // 10. Collapse visibility:collapse items.
    // TODO: Implement this step

    // 11. Determine the used cross size of each flex item.
    // TODO: Implement this step

    // 12. Distribute any remaining free space
    // TODO: Implement this step

    // 13. Resolve cross-axis auto margins.
    // TODO: Implement this step

    // 14. Align all flex items along the cross-axis.
    // TODO: Implement this step

    // 15. Determine the flex container's used cross size
    // TODO: Implement this step

    // 16. Align all flex lines
    // TODO: Implement this step

    // HACK: Bellow rough approximation to get something working

    Px mainSize = Px{0}, crossSize = Px{0};
    Px knownCrossSize = input.knownSize.y.unwrapOr(Px{0});

    for (auto &c : f.children()) {
        Opt<Px> childKnowCrossSize = NONE;
        if (c.style->sizing->width == Size::AUTO)
            childKnowCrossSize = knownCrossSize;

        auto ouput = layout(
            t,
            c,
            Input{
                .commit = input.commit,
                .knownSize = {NONE, childKnowCrossSize},
                .availableSpace = {Px{0}, knownCrossSize},
                .containingBlock = {Px{0}, knownCrossSize},
            }
        );

        mainSize += ouput.size.x;

        if (input.commit == Commit::YES)
            c.layout.position = {mainSize, Px{0}};

        crossSize = max(crossSize, ouput.size.y);
    }

    return Output::fromSize({mainSize, crossSize});
}

} // namespace Vaev::Layout
