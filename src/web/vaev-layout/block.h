#pragma once

#include "flow.h"
#include "sizing.h"

namespace Vaev::Layout {

struct BlockFlow : public Flow {
    static constexpr auto TYPE = BLOCK;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void placeChildren(Context &ctx, Box box) override {
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

    Px computeIntrinsicSize(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px) override {
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
};

} // namespace Vaev::Layout
