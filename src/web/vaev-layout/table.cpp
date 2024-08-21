#include "frag.h"
#include "sizing.h"

namespace Vaev::Layout {

struct ColumnSizing {
    Px width;

    void repr(Io::Emit &e) const {
        e("(column {})", width);
    }
};

static void _collectColumnSize(Tree &t, Frag &f, Input input, Vec<ColumnSizing> &cols, usize index = 0) {
    auto display = f->display;
    if (display == Display::TABLE_CELL) {
        if (cols.len() <= index) {
            cols.pushBack({
                .width = computePreferredOuterSize(t, f, input, Axis::HORIZONTAL),
            });
        } else {
            cols[index].width = max(
                cols[index].width,
                computePreferredOuterSize(t, f, input, Axis::HORIZONTAL)
            );
        }
        return;
    }

    for (auto &c : f.children()) {
        Input childInput = {
            .commit = Commit::NO,
            .axis = input.axis,
            .availableSpace = Px{0},
            .containingBlock = input.containingBlock,
        };

        if (display.isTableGroup() or display == Display::TABLE) {
            _collectColumnSize(t, c, childInput, cols);
            continue;
        }

        if (display == Display::TABLE_ROW) {
            _collectColumnSize(t, c, childInput, cols, index++);
            continue;
        }
    }
}

static Px _collectRowHeight(Tree &t, Frag &f, Input input) {
    auto display = f->display;
    if (display == Display::TABLE_CELL) {
        return computePreferredOuterSize(t, f, input, Axis::VERTICAL);
    }

    Px res = Px{0};
    for (auto &c : f.children()) {
        Input childInput = {
            .commit = Commit::NO,
            .axis = input.axis,
            .availableSpace = Px{0},
            .containingBlock = input.containingBlock,
        };

        res = max(res, _collectRowHeight(t, c, childInput));
    }

    return res;
}

static Px _collectRowsHeight(Tree &t, Frag &f, Input input) {
    Px res = Px{0};
    for (auto &c : f.children()) {
        Input childInput = {
            .commit = Commit::NO,
            .axis = input.axis,
            .availableSpace = Px{0},
            .containingBlock = input.containingBlock,
        };
        res += _collectRowHeight(t, c, childInput);
    }

    return res;
}

static Output _placeColumns(Tree &t, Frag &f, Box box, Input input, Vec<ColumnSizing> &cols) {
    if (input.commit == Commit::YES)
        f.box = box;

    Px res = box.contentBox().start();
    Px height = _collectRowHeight(t, f, input);

    for (usize i = 0; i < f.children().len(); i++) {
        auto &c = f.children()[i];

        RectPx borderBox = RectPx{
            res,
            box.contentBox().top(),
            cols[i].width,
            height,
        };

        Input childInput = {
            .commit = input.commit,
            .axis = input.axis,
            .availableSpace = borderBox.wh,
            .containingBlock = box.contentBox(),
        };

        auto childBox = computeBox(t, c, childInput, borderBox);
        layout(
            t, c,
            childBox,
            childInput
        );

        res += cols[i].width;
    }

    return Output::fromSize({
        res - box.contentBox().start(),
        height,
    });
}

static Output _placeRows(Tree &t, Frag &f, Box box, Input input, Vec<ColumnSizing> &cols) {
    if (input.commit == Commit::YES)
        f.box = box;

    Px res = box.contentBox().top();

    for (auto &c : f.children()) {
        Px blockSize = {};

        Input childInput = {
            .commit = input.commit,
            .axis = input.axis,
            .availableSpace = {},
            .containingBlock = box.contentBox(),
        };

        if (c->display == Display::TABLE_ROW)
            blockSize = _collectRowHeight(t, c, childInput);
        else
            blockSize = _collectRowsHeight(t, c, childInput);

        RectPx borderBox = RectPx{
            box.contentBox().start(),
            res,
            box.contentBox().width,
            blockSize,
        };

        auto childBox = computeBox(t, c, childInput, borderBox);

        if (c->display == Display::TABLE_ROW) {
            _placeColumns(t, c, childBox, input, cols);
        } else if (c->display == Display::TABLE_CAPTION) {
            layout(t, c, childBox, childInput);
        } else if (c->display.isTableGroup()) {
            _placeRows(t, c, childBox, input, cols);
        } else {
            layout(t, c, childBox, childInput);
        }

        res += blockSize;
    }

    return Output::fromSize({
        box.contentBox().width,
        res - box.contentBox().top(),
    });
}

Output tableLayout(Tree &t, Frag &f, Box box, Input input) {
    if (input.commit == Commit::YES)
        f.box = box;

    Vec<ColumnSizing> cols;
    _collectColumnSize(t, f, input, cols);
    return _placeRows(t, f, box, input, cols);
}

} // namespace Vaev::Layout
