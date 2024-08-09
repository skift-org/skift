#pragma once

#include "block.h"
#include "flow.h"
#include "sizing.h"

namespace Vaev::Layout {

// MARK: Base ------------------------------------------------------------------

struct ColumnSizing {
    Px width;

    void repr(Io::Emit &e) const {
        e("(column {})", width);
    }
};

// MARK: Cell ------------------------------------------------------------------

struct TableCell : public BlockFlow {
    static constexpr auto TYPE = TABLE_CELL;

    using BlockFlow::BlockFlow;

    Type type() const override {
        return TYPE;
    }
};

// MARK: Rows ------------------------------------------------------------------

struct TableRow : public Flow {
    static constexpr auto TYPE = TABLE_ROW;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void collectColumnSize(Context &ctx, Vec<ColumnSizing> &cols) {
        for (usize i = cols.len(); i < children().len(); i++) {
            auto &c = children()[i];
            auto childCtx = ctx.subContext(
                *c,
                Axis::HORIZONTAL,
                {}
            );

            cols.pushBack({
                .width = computePreferredOuterSize(ctx, Axis::HORIZONTAL),
            });
        }
    }

    void placeColumns(Context &ctx, Box box, Vec<ColumnSizing> &cols) {
        placeSelf(ctx, box);

        Px res = box.contentBox().start();

        for (usize i = 0; i < children().len(); i++) {
            auto &c = children()[i];

            auto childCtx = ctx.subContext(
                *c,
                Axis::HORIZONTAL,
                box.contentBox()
            );

            RectPx borderBox = RectPx{
                res,
                box.contentBox().top(),
                cols[i].width,
                box.contentBox().height,
            };

            auto box = computeBox(childCtx, borderBox);
            c->placeChildren(childCtx, box);

            res += cols[i].width;
        }
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

// MARK: Group -----------------------------------------------------------------

struct TableGroup : public Flow {
    static constexpr auto TYPE = TABLE_GROUP;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void collectColumnSize(Context &ctx, Vec<ColumnSizing> &cols) {
        if (style().display != Display::TABLE_ROW_GROUP and
            style().display != Display::TABLE_HEADER_GROUP and
            style().display != Display::TABLE_FOOTER_GROUP)
            return;

        for (auto &c : children()) {
            auto childCtx = ctx.subContext(
                *c,
                Axis::HORIZONTAL,
                {}
            );
            if (auto *row = c.is<TableRow>()) {
                row->collectColumnSize(childCtx, cols);
            }
        }
    }

    void placeColumns(Context &ctx, Box box, Vec<ColumnSizing> &cols) {
        placeSelf(ctx, box);

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

            if (auto *row = c.is<TableRow>()) {
                row->placeColumns(ctx, box, cols);
            } else {
                c->placeChildren(childcontext, box);
            }

            res += blockSize;
        }
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

// MARK: Table -----------------------------------------------------------------

struct TableFlow : public Flow {
    static constexpr auto TYPE = TABLE;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void collectColumnSize(Context &ctx, Vec<ColumnSizing> &cols) {
        for (auto &c : children()) {
            if (auto *groups = c.is<TableGroup>()) {
                groups->collectColumnSize(ctx, cols);
            }
        }
    }

    void placeChildren(Context &ctx, Box box) override {
        Frag::placeChildren(ctx, box);

        Axis mainAxis = Axis::VERTICAL;

        Px res = box.contentBox().top();

        Vec<ColumnSizing> cols;
        collectColumnSize(ctx, cols);
        logDebug("cols: {}", cols);

        for (auto &c : children()) {
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

            if (auto *groups = c.is<TableGroup>()) {
                groups->placeColumns(ctx, box, cols);
            } else {
                c->placeChildren(childcontext, box);
            }

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
