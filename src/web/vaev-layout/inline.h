#pragma once

#include "flow.h"
#include "sizing.h"

namespace Vaev::Layout {

struct InlineFlow : public Flow {
    static constexpr auto TYPE = INLINE;

    using Flow::Flow;

    struct Item {
        usize frag;
    };

    struct Line {
    };

    Vec<Item> _items;
    Vec<Line> _lines;

    Type type() const override {
        return TYPE;
    }

    void placeChildren(Context &ctx, Box box) override {
        Frag::placeChildren(ctx, box);

        Axis mainAxis = Axis::INLINE;

        Px res = box.borderBox.start();
        auto blockSize = computePreferredBorderSize(
            ctx,
            mainAxis.cross(),
            box.borderBox.height
        );

        for (auto &c : _frags) {
            auto childCtx = ctx.subContext(
                *c,
                mainAxis,
                box.borderBox
            );

            auto inlineSize = computePreferredOuterSize(
                childCtx, mainAxis,
                box.borderBox.width - res
            );

            RectPx borderBox = RectPx{
                res,
                box.borderBox.top(),
                inlineSize,
                blockSize,
            };

            auto box = computeBox(childCtx, borderBox);
            c->placeChildren(childCtx, box);

            res += inlineSize;
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
};

} // namespace Vaev::Layout
