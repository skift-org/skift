#include "block.h"

#include "frag.h"
#include "sizing.h"

namespace Vaev::Layout {

Output blockLayout(Tree &t, Frag &f, Box box, Input input) {
    if (input.commit == Commit::YES)
        f.box = box;

    Axis mainAxis = Axis::VERTICAL;

    Px pos = box.contentBox().top();
    Px width = box.contentBox().width;
    for (auto &c : f.children()) {
        auto blockSize = computePreferredOuterSize(
            t, c,
            input,
            mainAxis,
            max(Px{0}, box.contentBox().height - pos)
        );

        Px inlineSize = computePreferredBorderSize(
            t, c,
            input,
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

        pos += blockSize;
        width = max(width, childBox.borderBox.width);
    }

    return Output::fromSize({
        width,
        pos - box.contentBox().top(),
    });
}

} // namespace Vaev::Layout
