#include "flex.h"

#include "sizing.h"

namespace Vaev::Layout {

void flexLayout(Context &ctx, Box box) {
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

        auto inlineSize = computePreferredOuterSize(
            childContext, mainAxis,
            max(Px{0}, box.contentBox().width - res)
        );

        RectPx borderBox = RectPx{
            res,
            box.contentBox().top(),
            inlineSize,
            blockSize,
        };

        auto box = computeBox(childContext, borderBox);
        layout(childContext, box);

        res += inlineSize;
    }
}

Px flexMeasure(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px) {
    Px res = Px{};

    for (auto &c : ctx.children()) {
        auto childCtx = ctx.subContext(
            c,
            axis,
            Vec2Px::ZERO
        );

        if (axis == Axis::HORIZONTAL) {
            auto size = computePreferredOuterSize(childCtx, axis);
            if (intrinsic == IntrinsicSize::MAX_CONTENT) {
                res += size;
            } else {
                res = max(res, size);
            }
        } else {
            auto size = computePreferredOuterSize(childCtx, axis);
            res = max(res, size);
        }
    }

    return res;
}

} // namespace Vaev::Layout
