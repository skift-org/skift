#include "flex.h"

#include "sizing.h"

namespace Vaev::Layout {

Output flexLayout(Context &ctx, Box box, Input input) {
    if (input.commit == Commit::YES)
        ctx.frag.box = box;

    Axis mainAxis = Axis::HORIZONTAL;

    Px res = box.contentBox().start();
    auto blockSize = computePreferredBorderSize(
        ctx,
        mainAxis.cross(),
        box.contentBox().height
    );

    for (auto &c : ctx.children()) {
        auto childContext = ctx.subContext(
            c,
            mainAxis,
            box.contentBox()
        );

        auto availableSpace = max(Px{0}, box.contentBox().width - res);

        auto inlineSize = computePreferredOuterSize(
            childContext, mainAxis,
            availableSpace
        );

        RectPx borderBox = RectPx{
            res,
            box.contentBox().top(),
            inlineSize,
            blockSize,
        };

        auto box = computeBox(childContext, borderBox);
        layout(childContext, box, input.withAvailableSpace(borderBox.wh));

        res += inlineSize;
    }

    return Output::fromSize({
        res - box.contentBox().start(),
        blockSize,
    });
}

} // namespace Vaev::Layout
