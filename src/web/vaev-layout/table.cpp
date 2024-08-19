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
        } else {
            cols[index].width = max(
                cols[index].width,
                computePreferredOuterSize(ctx, Axis::HORIZONTAL)
            );
        }
        return;
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

static Px _collectRowHeight(Context &ctx) {
    auto display = ctx.style().display;
    if (display == Display::TABLE_CELL) {
        return computePreferredOuterSize(ctx, Axis::VERTICAL);
    }

    Px res = Px{0};
    for (auto &c : ctx.children()) {
        auto childCtx = ctx.subContext(
            c,
            Axis::HORIZONTAL,
            {}
        );

        res = max(res, _collectRowHeight(childCtx));
    }

    return res;
}

static Px _collectRowsHeight(Context &ctx) {
    Px res = Px{0};
    for (auto &c : ctx.children()) {
        auto childCtx = ctx.subContext(
            c,
            Axis::HORIZONTAL,
            {}
        );

        res += _collectRowHeight(childCtx);
    }

    return res;
}

static Output _placeColumns(Context &ctx, Box box, Input input, Vec<ColumnSizing> &cols) {
    if (input.commit == Commit::YES)
        ctx.frag.box = box;

    Px res = box.contentBox().start();
    Px height = _collectRowHeight(ctx);

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
            height,
        };

        auto box = computeBox(childCtx, borderBox);
        layout(childCtx, box, input.withAvailableSpace(borderBox.wh));

        res += cols[i].width;
    }

    return Output::fromSize({
        res - box.contentBox().start(),
        height,
    });
}

static Output _placeRows(Context &ctx, Box box, Input input, Vec<ColumnSizing> &cols) {
    if (input.commit == Commit::YES)
        ctx.frag.box = box;

    Axis mainAxis = Axis::VERTICAL;

    Px res = box.contentBox().top();

    for (auto &c : ctx.children()) {
        auto childContext = ctx.subContext(
            c,
            mainAxis,
            box.contentBox()
        );

        Px blockSize = {};

        if (c->display == Display::TABLE_ROW)
            blockSize = _collectRowHeight(childContext);
        else
            blockSize = _collectRowsHeight(childContext);

        RectPx borderBox = RectPx{
            box.contentBox().start(),
            res,
            box.contentBox().width,
            blockSize,
        };
        auto childBox = computeBox(childContext, borderBox);

        if (c->display == Display::TABLE_ROW) {
            _placeColumns(childContext, childBox, input, cols);
        } else if (c->display == Display::TABLE_CAPTION) {
            layout(childContext, childBox, input.withAvailableSpace(borderBox.wh));
        } else if (c->display.isTableGroup()) {
            _placeRows(childContext, childBox, input, cols);
        } else {
            layout(childContext, childBox, input.withAvailableSpace(borderBox.wh));
        }

        res += blockSize;
    }

    return Output::fromSize({
        box.contentBox().width,
        res - box.contentBox().top(),
    });
}

Output tableLayout(Context &ctx, Box box, Input input) {
    if (input.commit == Commit::YES)
        ctx.frag.box = box;

    Vec<ColumnSizing> cols;
    _collectColumnSize(ctx, cols);
    return _placeRows(ctx, box, input, cols);
}

} // namespace Vaev::Layout
