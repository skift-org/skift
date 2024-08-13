#include "frag.h"
#include "grid.h"
#include "sizing.h"

namespace Vaev::Layout {

struct ColumnSizing {
    Px width;

    void repr(Io::Emit &e) const {
        e("(column {})", width);
    }
};

static void _collectColumnSize(Context &ctx, Vec<ColumnSizing> &cols, usize index = 0) {
    auto display = ctx.style().display;
    if (display == Display::TABLE_CELL) {
        if (cols.len() <= index) {
            cols.pushBack({
                .width = computePreferredOuterSize(ctx, Axis::HORIZONTAL),
            });
        }
    }

    for (auto &c : ctx.children()) {
        auto childCtx = ctx.subContext(
            c,
            Axis::HORIZONTAL,
            {}
        );

        if (display.isTableGroup() or display == Display::TABLE) {
            _collectColumnSize(childCtx, cols);
            continue;
        }

        if (display == Display::TABLE_ROW) {
            _collectColumnSize(childCtx, cols, index++);
            continue;
        }
    }
}

static void _placeColumns(Context &ctx, Box box, Vec<ColumnSizing> &cols) {
    ctx.frag.box = box;

    Px res = box.contentBox().start();

    for (usize i = 0; i < ctx.children().len(); i++) {
        auto &c = ctx.children()[i];

        auto childCtx = ctx.subContext(
            c,
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
        layout(childCtx, box);

        res += cols[i].width;
    }
}

void _placeRows(Context &ctx, Box box, Vec<ColumnSizing> &cols) {
    ctx.frag.box = box;

    Axis mainAxis = Axis::VERTICAL;

    Px res = box.contentBox().top();

    for (auto &c : ctx.children()) {
        auto childContext = ctx.subContext(
            c,
            mainAxis,
            box.contentBox()
        );

        auto blockSize = computePreferredOuterSize(
            childContext,
            mainAxis,
            max(Px{0}, box.contentBox().height - res)
        );

        RectPx borderBox = RectPx{
            box.contentBox().start(),
            res,
            box.contentBox().width,
            blockSize,
        };

        auto box = computeBox(childContext, borderBox);

        if (c->display == Display::TABLE_ROW) {
            _placeColumns(childContext, box, cols);
        } else if (c->display.isTableGroup()) {
            _placeRows(childContext, box, cols);
        } else {
            layout(childContext, box);
        }

        res += blockSize;
    }
}

void tableLayout(Context &ctx, Box box) {
    ctx.frag.box = box;

    Vec<ColumnSizing> cols;
    _collectColumnSize(ctx, cols);
    _placeRows(ctx, box, cols);
}

Px tableMeasure(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px) {
    Px res = Px{};

    for (auto &c : ctx.children()) {
        auto childCtx = ctx.subContext(
            c,
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
