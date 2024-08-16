#include "block.h"

#include "frag.h"
#include "sizing.h"

namespace Vaev::Layout {

Output blockLayout(Context &ctx, Box box, Input input) {
    if (input.commit == Commit::YES)
        ctx.frag.box = box;

    Axis mainAxis = Axis::VERTICAL;

    Px pos = box.contentBox().top();
    Px width = box.contentBox().width;
    for (auto &c : ctx.children()) {
        auto childcontext = ctx.subContext(
            c,
            mainAxis,
            box.contentBox()
        );

        auto blockSize = computePreferredOuterSize(
            childcontext,
            mainAxis,
            max(Px{0}, box.contentBox().height - pos)
        );

        Px inlineSize = computePreferredBorderSize(
            childcontext,
            mainAxis.cross(),
            box.contentBox().width
        );

        if (c->sizing->width == Size::AUTO) {
            inlineSize = max(inlineSize, box.contentBox().width);
        }

        RectPx borderBox = RectPx{
            box.contentBox().start(),
            pos,
            inlineSize,
            blockSize,
        };

        auto box = computeBox(childcontext, borderBox);
        layout(childcontext, box, input.withAvailableSpace(borderBox.wh));

        pos += blockSize;
        width = max(width, box.contentBox().width);
    }

    return Output::fromSize({
        width,
        pos - box.contentBox().top(),
    });
}

} // namespace Vaev::Layout
