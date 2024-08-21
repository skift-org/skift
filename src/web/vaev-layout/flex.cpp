#include "flex.h"

#include "sizing.h"

namespace Vaev::Layout {

Output flexLayout(Tree &t, Frag &f, Box box, Input input) {
    if (input.commit == Commit::YES)
        f.box = box;

    Axis mainAxis = Axis::HORIZONTAL;

    Px res = box.contentBox().start();
    auto blockSize = computePreferredBorderSize(
        t, f,
        input,
        mainAxis.cross(),
        box.contentBox().height
    );

    for (auto &c : f.children()) {
        auto availableSpace = max(Px{0}, box.contentBox().width - res);

        auto inlineSize = computePreferredOuterSize(
            t, f,
            input,
            mainAxis,
            availableSpace
        );

        RectPx borderBox = RectPx{
            res,
            box.contentBox().top(),
            inlineSize,
            blockSize,
        };

        auto childBox = computeBox(t, c, input, borderBox);

        layout(
            t, c,
            childBox,
            {
                .commit = input.commit,
                .axis = input.axis,
                .availableSpace = borderBox.wh,
                .containingBlock = box.contentBox(),
            }
        );

        res += inlineSize;
    }

    return Output::fromSize({
        res - box.contentBox().start(),
        blockSize,
    });
}

} // namespace Vaev::Layout
