#include "block.h"

#include "sizing.h"

namespace Vaev::Layout {

void BlockFlow::placeChildren(Context &ctx, Box box) {
    Frag::placeChildren(ctx, box);

    Axis mainAxis = Axis::VERTICAL;

    Px res = box.contentBox().start();

    for (auto &c : _frags) {
        auto childcontext = ctx.subContext(
            *c,
            mainAxis,
            box.contentBox()
        );

        auto blockSize = computePreferredOuterSize(
            childcontext,
            mainAxis,
            max(Px{0}, box.contentBox().height - res)
        );

        auto inlineSize = computePreferredBorderSize(
            childcontext,
            mainAxis.cross(),
            box.contentBox().width
        );

        RectPx borderBox = RectPx{
            box.contentBox().start(),
            res,
            inlineSize,
            blockSize,
        };

        auto box = computeBox(childcontext, borderBox);
        c->placeChildren(childcontext, box);

        res += blockSize;
    }
}

Px BlockFlow::computeIntrinsicSize(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px) {
    Px res = Px{};

    for (auto &c : _frags) {
        auto childCtx = ctx.subContext(
            *c,
            axis,
            Vec2Px::ZERO
        );

        if (axis == Axis::VERTICAL) {
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
