#include "table.h"

#include "frag.h"

namespace Vaev::Layout {

struct TableColumn {
    Px width;

    void repr(Io::Emit &e) const {
        e("(column {})", width);
    }
};

static void _collectColumnSize(Tree &t, Frag &f, Input input, Vec<TableColumn> &cols, usize col = 0) {
    auto display = f.style->display;
    if (display == Display::TABLE_CELL) {
        Input childInput = {
            .commit = Commit::NO,
            .containingBlock = input.containingBlock
        };

        if (cols.len() <= col) {
            cols.pushBack({
                .width = layout(t, f, childInput).size.width,
            });
        } else {
            cols[col].width = max(
                cols[col].width,
                layout(t, f, childInput).size.width
            );
        }
        return;
    }

    for (auto &c : f.children()) {
        Input childInput = {
            .commit = Commit::NO,
            .containingBlock = input.containingBlock,
        };

        if (display.isTableGroup() or display == Display::TABLE) {
            _collectColumnSize(t, c, childInput, cols);
            continue;
        }

        if (display == Display::TABLE_ROW) {
            _collectColumnSize(t, c, childInput, cols, col++);
            continue;
        }
    }
}

static Px _collectRowHeight(Tree &t, Frag &f, Input input) {
    auto display = f.style->display;

    if (display == Display::TABLE_CELL) {
        Input childInput = {
            .commit = Commit::NO,
            .containingBlock = input.containingBlock,
        };
        return layout(t, f, childInput).size.height;
    }

    Px res = Px{0};
    for (auto &c : f.children()) {
        Input childInput = {
            .commit = Commit::NO,
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
            .availableSpace = Px{0},
            .containingBlock = input.containingBlock,
        };
        res += _collectRowHeight(t, c, childInput);
    }

    return res;
}

static Output _placeColumns(Tree &t, Frag &f, Input input, Vec<TableColumn> &cols) {
    Px inlineSize = {};
    Px rowBlockSize = _collectRowHeight(t, f, input);

    for (usize i = 0; i < f.children().len(); i++) {
        auto &c = f.children()[i];

        Input childInput = {
            .commit = input.commit,
            .knownSize = {
                cols[i].width,
                rowBlockSize,
            },
            .availableSpace = {},
            .containingBlock = input.containingBlock,
        };

        auto output = layout(
            t, c,
            childInput
        );

        c.layout.position = {
            inlineSize,
            Px{0},
        };

        inlineSize += cols[i].width;
    }

    return Output::fromSize({
        inlineSize,
        rowBlockSize,
    });
}

static Output _placeRows(Tree &t, Frag &f, Input input, Vec<TableColumn> &cols) {
    Px blockSize = {};
    Px knownInlineSize = input.knownSize.x.unwrapOr(Px{0});

    for (auto &c : f.children()) {
        auto display = c.style->display;

        Px childBlockSize = {};

        Input childInput = {
            .commit = input.commit,
            .knownSize = {knownInlineSize, NONE},
            .availableSpace = {},
            .containingBlock = {
                knownInlineSize,
                Px{0},
            }
        };

        if (display == Display::TABLE_ROW)
            childBlockSize = _collectRowHeight(t, c, childInput);
        else
            childBlockSize = _collectRowsHeight(t, c, childInput);

        input.availableSpace.height = childBlockSize;

        if (display == Display::TABLE_ROW) {
            _placeColumns(t, c, input, cols);
        } else if (display == Display::TABLE_CAPTION) {
            layout(t, c, childInput);
        } else if (display.isTableGroup()) {
            _placeRows(t, c, input, cols);
        } else {
            layout(t, c, childInput);
        }

        c.layout.position = {
            Px{0},
            blockSize,
        };

        blockSize += childBlockSize;
    }

    auto inlineSize =
        iter(cols)
            .map([](auto &col) {
                return col.width;
            })
            .sum();

    return Output::fromSize({
        inlineSize,
        blockSize,
    });
}

Output tableLayout(Tree &t, Frag &f, Input input) {
    Vec<TableColumn> cols;
    _collectColumnSize(t, f, input, cols);
    return _placeRows(t, f, input, cols);
}

} // namespace Vaev::Layout
