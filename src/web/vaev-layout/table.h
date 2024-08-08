#pragma once

#include "flow.h"
#include "sizing.h"

namespace Vaev::Layout {

struct TableCell : public Flow {
    static constexpr auto TYPE = TABLE_CELL;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void placeChildren(Context &ctx, Box box) override {
        Frag::placeChildren(ctx, box);

        Axis mainAxis = Axis::INLINE;

        Px res = box.contentBox().start();

        for (auto &c : _frags) {
            auto childCtx = ctx.subContext(
                *c,
                mainAxis,
                box.contentBox()
            );

            auto inlineSize = computePreferredOuterSize(
                childCtx,
                mainAxis,
                max(Px{0}, box.contentBox().width - res)
            );

            auto blockSize = computePreferredBorderSize(
                childCtx,
                mainAxis.cross(),
                box.contentBox().height
            );

            RectPx borderBox = RectPx{
                res,
                box.contentBox().top(),
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

struct TableRow : public Flow {
    static constexpr auto TYPE = TABLE_ROW;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void placeChildren(Context &ctx, Box box) override {
        Frag::placeChildren(ctx, box);

        Axis mainAxis = Axis::INLINE;

        Px res = box.contentBox().start();
        auto blockSize = computePreferredBorderSize(
            ctx,
            mainAxis.cross(),
            box.contentBox().height
        );

        for (auto &c : _frags) {
            auto childCtx = ctx.subContext(
                *c,
                mainAxis,
                box.contentBox()
            );

            auto inlineSize = computePreferredOuterSize(
                childCtx, mainAxis,
                box.contentBox().width - res
            );

            RectPx borderBox = RectPx{
                res,
                box.contentBox().top(),
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

struct TableGroup : public Flow {
    static constexpr auto TYPE = TABLE_GROUP;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void placeChildren(Context &ctx, Box box) override {
        Frag::placeChildren(ctx, box);

        Axis mainAxis = Axis::VERTICAL;

        Px res = box.contentBox().top();

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

            RectPx borderBox = RectPx{
                box.contentBox().start(),
                res,
                box.contentBox().width,
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

struct TableFlow : public Flow {
    static constexpr auto TYPE = TABLE;

    using Flow::Flow;

    struct Dim {
        Px size;
    };

    Vec<Dim> _rows;
    Px _rowSize;

    Vec<Dim> _cols;
    Px _colSize;

    Type type() const override {
        return TYPE;
    }

    void placeChildren(Context &ctx, Box box) override {
        Frag::placeChildren(ctx, box);

        Axis mainAxis = Axis::VERTICAL;

        Px res = box.contentBox().top();

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

            RectPx borderBox = RectPx{
                box.contentBox().start(),
                res,
                box.contentBox().width,
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
