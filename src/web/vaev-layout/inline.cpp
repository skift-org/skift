#include "inline.h"

namespace Vaev::Layout {

void InlineFlow::placeChildren(Context &ctx, Box box) {
    Frag::placeChildren(ctx, box);

    Axis mainAxis = Axis::INLINE;

    Px res = box.contentBox().start();

    auto blockSize = computePreferredBorderSize(
        ctx,
        mainAxis.cross(),
        box.contentBox().height
    );

    for (auto &c : _frags) {
        auto childContext = ctx.subContext(
            *c,
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
        c->placeChildren(childContext, box);

        res += inlineSize;
    }
}

Px InlineFlow::computeIntrinsicSize(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px) {
    Px res = Px{};

    for (auto &c : _frags) {
        auto childCtx = ctx.subContext(
            *c,
            axis,
            Vec2Px::ZERO
        );

        if (axis == Axis::HORIZONTAL) {
            auto size = computePreferredOuterSize(childCtx, axis, Px{0});
            if (intrinsic == IntrinsicSize::MAX_CONTENT) {
                res += size;
            } else {
                res = max(res, size);
            }
        } else {
            auto size = computePreferredOuterSize(childCtx, axis, Px{0});
            res = max(res, size);
        }
    }

    return res;
}

} // namespace Vaev::Layout
