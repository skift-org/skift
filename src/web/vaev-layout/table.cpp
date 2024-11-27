#include "table.h"

#include "box.h"
#include "layout.h"
#include "values.h"

namespace Vaev::Layout {

void advanceUntil(MutCursor<Box> &cursor, Func<bool(Display)> pred) {
    while (not cursor.ended() and not pred(cursor->style->display))
        cursor.next();
}

struct TableCell {
    Math::Vec2u anchorIdx = {};
    MutCursor<Box> box = nullptr;

    static TableCell const EMPTY;

    bool operator==(TableCell const &c) const {
        return box == c.box and anchorIdx == c.anchorIdx;
    }

    bool isOccupied() const {
        return box != nullptr;
    }
};

struct TableAxis {
    usize start, end;
    Box &el;
};

struct TableGroup {
    usize start, end;
    Box &el;
};

struct TableGrid {
    Vec<Vec<TableCell>> rows;
    Math::Vec2u size = {0, 0};

    void increaseWidth(usize span = 1) {
        for (auto &row : rows) {
            for (usize i = 0; i < span; ++i)
                row.pushBack({});
        }
        size.x += span;
    }

    void increaseHeight(usize span = 1) {
        Vec<TableCell> newRow;
        newRow.resize(size.x);
        for (usize i = 0; i < span; ++i)
            rows.pushBack(newRow);
        size.y += span;
    }

    TableCell &get(usize x, usize y) {
        if (x >= size.x or y >= size.y)
            panic("bad coordinates for table slot");

        return rows[y][x];
    }

    void set(usize x, usize y, TableCell cell) {
        if (x >= size.x or y >= size.y)
            panic("bad coordinates for table slot");

        rows[y][x] = cell;
    }
};

template <typename T>
struct PrefixSum {

    Vec<Px> pref;

    PrefixSum(Vec<Px> const &v) : pref(v) {
        for (usize i = 1; i < v.len(); ++i)
            pref[i] = pref[i - 1] + pref[i];
    }

    T query(isize l, isize r) {
        if (r < l)
            return T{};
        if (l == 0)
            return pref[r];
        return pref[r] - pref[l - 1];
    }
};

struct TableFormatingContext {
    TableGrid grid;

    Vec<TableAxis> cols;
    Vec<TableAxis> rows;
    Vec<TableGroup> rowGroups;
    Vec<TableGroup> colGroups;

    Vec<usize> captionsIdxs;
    Box &wrapperBox;
    Box &tableBox;

    // Table forming algorithm
    Math::Vec2u current;

    struct DownwardsGrowingCell {
        Math::Vec2u cellIdx;
        usize xpos, width;
    };

    Vec<DownwardsGrowingCell> downwardsGrowingCells;
    Vec<usize> pendingTfoots;

    // TODO: amount of footers and headers
    // footers will be the last rows of the grid; same for headers?
    usize numOfHeaderRows = 0, numOfFooterRows = 0;

    struct {
        usize width;
        Vec<InsetsPx> borders;

        InsetsPx &get(usize i, usize j) {
            return borders[i * width + j];
        }
    } bordersGrid;

    InsetsPx boxBorder;
    Vec2Px spacing;

    TableFormatingContext(Tree &tree, Box &tableWrapperBox)
        : wrapperBox(tableWrapperBox),
          tableBox(findTableBox(tableWrapperBox)),
          boxBorder(computeBorders(tree, tableBox)),
          spacing(
              {
                  resolve(tree, tableBox, tableBox.style->table->spacing.horizontal),
                  resolve(tree, tableBox, tableBox.style->table->spacing.vertical),
              }
          ) {

        for (usize i = 0; i < tableWrapperBox.children().len(); ++i) {
            auto &child = tableWrapperBox.children()[i];
            if (child.style->display == Display::Internal::TABLE_CAPTION) {
                captionsIdxs.pushBack(i);
            }
        }
    }

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-growing-downward-growing-cells
    void growDownwardGrowingCells() {
        for (auto &[cellIdx, cellx, width] : downwardsGrowingCells) {
            for (usize x = cellx; x < cellx + width; x++)
                grid.set(x, current.y, grid.get(cellIdx.x, cellIdx.y));
        }
    }

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-processing-rows
    void processRows(Box &tableRowElement) {
        auto tableRowChildren = tableRowElement.children();
        MutCursor<Box> tableRowCursor{tableRowChildren};

        if (grid.size.y == current.y)
            grid.increaseHeight();

        current.x = 0;

        growDownwardGrowingCells();

        advanceUntil(tableRowCursor, [](Display d) {
            return d == Display::TABLE_CELL;
        });

        while (not tableRowCursor.ended()) {
            while (current.x < grid.size.x and grid.get(current.x, current.y).isOccupied())
                current.x++;

            if (current.x == grid.size.x)
                grid.increaseWidth();

            usize rowSpan = tableRowCursor->attrs.rowSpan;
            usize colSpan = tableRowCursor->attrs.colSpan;

            bool cellGrowsDownward;
            if (rowSpan == 0 and true /* TODO: and the table element's node document is not set to quirks mode, */) {
                cellGrowsDownward = true;
                rowSpan = 1;
            } else
                cellGrowsDownward = false;

            if (grid.size.x < current.x + colSpan) {
                grid.increaseWidth(current.x + colSpan - grid.size.x);
            }

            if (grid.size.y < current.y + rowSpan) {
                grid.increaseHeight(current.y + rowSpan - grid.size.y);
            }

            {
                TableCell cell = {
                    .anchorIdx = current,
                    .box = tableRowCursor,
                };

                for (usize x = current.x; x < current.x + colSpan; ++x) {
                    for (usize y = current.y; y < current.y + rowSpan; ++y) {
                        grid.set(x, y, cell);
                    }
                }

                if (cellGrowsDownward) {
                    downwardsGrowingCells.pushBack({
                        .cellIdx = current,
                        .xpos = current.x,
                        .width = colSpan,
                    });
                }
            }

            current.x += colSpan;

            tableRowCursor.next();
            advanceUntil(tableRowCursor, [](Display d) {
                return d == Display::TABLE_CELL;
            });
        }
        current.y++;
    }

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-ending-a-row-group
    void endRowGroup() {
        while (current.y < grid.size.y) {
            growDownwardGrowingCells();
            current.y++;
            downwardsGrowingCells.clear();
        }
    }

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-processing-row-groups
    void processRowGroup(Box &rowGroupElement) {
        auto rowGroupChildren = rowGroupElement.children();
        MutCursor<Box> rowGroupCursor{rowGroupChildren};

        usize ystartRowGroup = grid.size.y;

        advanceUntil(rowGroupCursor, [](Display d) {
            return d == Display::TABLE_ROW;
        });

        while (not rowGroupCursor.ended()) {
            usize ystartRow = grid.size.y;
            processRows(*rowGroupCursor);
            rows.pushBack({.start = ystartRow, .end = grid.size.y - 1, .el = *rowGroupCursor});

            rowGroupCursor.next();
            advanceUntil(rowGroupCursor, [](Display d) {
                return d == Display::TABLE_ROW;
            });
        }

        if (grid.size.y > ystartRowGroup) {
            rowGroups.pushBack({.start = ystartRowGroup, .end = grid.size.y - 1, .el = rowGroupElement});
        }

        endRowGroup();
    }

    Opt<usize> findFirstHeader() {
        auto tableBoxChildren = tableBox.children();
        MutCursor<Box> tableBoxCursor{tableBoxChildren};

        advanceUntil(tableBoxCursor, [](Display d) {
            return d == Display::TABLE_HEADER_GROUP;
        });

        if (tableBoxCursor.ended())
            return NONE;

        return tableBoxCursor - tableBoxChildren.begin();
    }

    // https://html.spec.whatwg.org/multipage/tables.html#forming-a-table
    void buildHTMLTable() {
        auto indexOfHeaderGroup = findFirstHeader();

        auto tableBoxChildren = tableBox.children();
        MutCursor<Box> tableBoxCursor{tableBoxChildren};

        if (tableBoxCursor.ended())
            return;

        advanceUntil(tableBoxCursor, [](Display d) {
            return d.isHeadBodyFootRowOrColGroup();
        });

        if (tableBoxCursor.ended())
            return;

        // MARK: Columns groups
        while (not tableBoxCursor.ended() and tableBoxCursor->style->display == Display::TABLE_COLUMN_GROUP) {
            auto columnGroupChildren = tableBoxCursor->children();
            MutCursor<Box> columnGroupCursor = {columnGroupChildren};

            advanceUntil(columnGroupCursor, [](Display d) {
                return d == Vaev::Display::TABLE_COLUMN;
            });

            if (not columnGroupCursor.ended()) {
                usize startColRange = grid.size.x;

                // MARK: Columns
                while (not columnGroupCursor.ended()) {
                    auto span = columnGroupCursor->attrs.span;
                    grid.increaseWidth(span);

                    cols.pushBack({.start = grid.size.x - span, .end = grid.size.x - 1, .el = *columnGroupCursor});

                    columnGroupCursor.next();
                    advanceUntil(columnGroupCursor, [](Display d) {
                        return d == Vaev::Display::TABLE_COLUMN;
                    });
                }

                colGroups.pushBack({.start = startColRange, .end = grid.size.x - 1, .el = *tableBoxCursor});
            } else {
                auto span = tableBoxCursor->attrs.span;
                grid.increaseWidth(span);

                colGroups.pushBack({.start = grid.size.x - span + 1, .end = grid.size.x - 1, .el = *tableBoxCursor});
            }

            tableBoxCursor.next();
            advanceUntil(tableBoxCursor, [](Display d) {
                return d.isHeadBodyFootRowOrColGroup();
            });
        }

        current.y = 0;

        // MARK: Rows

        if (indexOfHeaderGroup) {
            processRowGroup(tableBox.children()[indexOfHeaderGroup.unwrap()]);
            numOfHeaderRows = grid.size.y;
        }

        while (true) {
            advanceUntil(tableBoxCursor, [](Display d) {
                return d.isHeadBodyFootOrRow();
            });

            if (tableBoxCursor.ended())
                break;

            if (tableBoxCursor->style->display == Display::TABLE_ROW) {
                processRows(*tableBoxCursor);
                tableBoxCursor.next();
                continue;
            }

            endRowGroup();

            if (tableBoxCursor->style->display == Display::TABLE_FOOTER_GROUP) {
                pendingTfoots.pushBack(tableBoxCursor - tableBoxChildren.begin());
                tableBoxCursor.next();
                continue;
            }

            // The current element is either a thead or a tbody.
            if (tableBoxCursor->style->display != Display::TABLE_HEADER_GROUP and
                tableBoxCursor->style->display != Display::TABLE_ROW_GROUP) {
                // FIXME: Prod code should not fail, but ok for current dev scenario
                panic("current element should be thead or tbody");
            }

            if (indexOfHeaderGroup and
                indexOfHeaderGroup.unwrap() == (usize)(tableBoxCursor - tableBoxChildren.begin())) {
                // table header was already processed in the beggining of the Rows section of the algorithm
                tableBoxCursor.next();
                continue;
            }

            processRowGroup(*tableBoxCursor);

            tableBoxCursor.next();
        }

        usize ystartFooterRows = grid.size.y;
        for (auto tfootIdx : pendingTfoots) {
            processRowGroup(tableBoxChildren[tfootIdx]);
        }
        numOfFooterRows = grid.size.y - ystartFooterRows;
    }

    Box &findTableBox(Box &tableWrapperBox) {
        for (auto &child : tableWrapperBox.children())
            if (child.style->display != Display::Internal::TABLE_CAPTION)
                return child;

        panic("table box not found in box tree");
    }

    void buildBordersGrid(Tree &tree) {
        bordersGrid.borders.clear();
        bordersGrid.borders.resize(grid.size.x * grid.size.y);

        bordersGrid.width = grid.size.x;

        for (usize i = 0; i < grid.size.y; ++i) {
            for (usize j = 0; j < grid.size.x; ++j) {
                auto &cell = grid.get(j, i);
                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                usize rowSpan = cell.box->attrs.rowSpan;
                usize colSpan = cell.box->attrs.colSpan;

                auto cellBorder = computeBorders(tree, *cell.box);

                // Top and bottom borders
                for (usize k = 0; k < rowSpan; ++k) {
                    bordersGrid.get(i, j + k).top = cellBorder.top;
                    bordersGrid.get(i + rowSpan - 1, j + k).bottom = cellBorder.bottom;
                }

                // Left and right borders
                for (usize k = 0; k < rowSpan; ++k) {
                    bordersGrid.get(i + k, j).start = cellBorder.start;
                    bordersGrid.get(i + k, j + colSpan - 1).end = cellBorder.end;
                }
            }
        }
    }

    // https://www.w3.org/TR/CSS22/tables.html#borders
    Tuple<Vec<Px>, Px> getColumnBorders() {
        // NOTE: Borders are only defined for cells and Table Box
        //       Rows, columns, row groups, and column groups cannot have borders
        //       (i.e., user agents must ignore the border properties for those elements).

        Vec<Px> borders;
        Px sumBorders{0};

        for (usize j = 0; j < grid.size.x; ++j) {
            Px columnBorder{0};

            for (usize i = 0; i < grid.size.y; ++i) {
                auto cell = grid.get(j, i);

                columnBorder = max(
                    columnBorder,
                    bordersGrid.get(i, j).horizontal()
                );
            }

            borders.pushBack(columnBorder);
            sumBorders += columnBorder;
        }
        return {borders, sumBorders};
    }

    Vec<Px> colWidth;
    Px tableUsedWidth;

    // MARK: Fixed Table Layout
    // https://www.w3.org/TR/CSS22/tables.html#fixed-table-layout

    void computeFixedColWidths(Tree &tree, Input &input) {
        // NOTE: Percentages for 'width' and 'height' on the table (box)
        //       are calculated relative to the containing block of the
        //       table wrapper box, not the table wrapper box itself.
        //       (See https://www.w3.org/TR/CSS22/tables.html#model)

        // NOTE: The table does not automatically expand to fill its containing block.
        //       (https://www.w3.org/TR/CSS22/tables.html#width-layout)

        tableUsedWidth =
            tableBox.style->sizing->width == Size::AUTO
                ? 0_px
                : resolve(tree, tableBox, tableBox.style->sizing->width.value, input.availableSpace.x);

        auto [columnBorders, sumBorders] = getColumnBorders();

        Px fixedWidthToAccount = boxBorder.horizontal() + Px{grid.size.x + 1} * spacing.x;

        Vec<Opt<Px>> colWidthOrNone{};
        colWidthOrNone.resize(grid.size.x);
        for (auto &col : cols) {

            auto width = col.el.style->sizing->width;
            if (width == Size::AUTO)
                continue;

            for (usize x = col.start; x <= col.end; ++x) {
                colWidthOrNone[x] = resolve(tree, col.el, width.value, tableUsedWidth);
            }
        }

        // Using first row cells to define columns widths

        usize x = 0;
        while (x < grid.size.x) {
            auto cell = grid.get(x, 0);
            if (cell.box->style->sizing->width == Size::Type::AUTO) {
                x++;
                continue;
            }

            if (cell.anchorIdx != Math::Vec2u{x, 0})
                continue;

            auto cellWidth = resolve(tree, *cell.box, cell.box->style->sizing->width.value, tableUsedWidth);
            auto colSpan = cell.box->attrs.colSpan;

            for (usize j = 0; j < colSpan; ++j, x++) {
                // FIXME: Not overriding values already computed,
                //        but should we subtract the already computed from
                //        cellWidth before division?
                if (colWidthOrNone[x] == NONE)
                    colWidthOrNone[x] = cellWidth / Px{colSpan};
            }
        }

        Px sumColsWidths{0};
        usize emptyCols{0};
        for (usize i = 0; i < grid.size.x; ++i) {
            if (colWidthOrNone[i])
                sumColsWidths += colWidthOrNone[i].unwrap() + columnBorders[i];
            else
                emptyCols++;
        }

        if (emptyCols > 0) {
            if (sumColsWidths < tableUsedWidth - fixedWidthToAccount) {
                Px toDistribute = (tableUsedWidth - fixedWidthToAccount - sumColsWidths) / Px{emptyCols};
                for (auto &w : colWidthOrNone)
                    if (w == NONE)
                        w = toDistribute;
            }
        } else if (sumColsWidths < tableUsedWidth - fixedWidthToAccount) {
            Px toDistribute = (tableUsedWidth - fixedWidthToAccount - sumColsWidths);
            for (auto &w : colWidthOrNone) {
                w = w.unwrap() + (toDistribute * w.unwrap()) / sumColsWidths;
            }
        }

        colWidth.ensure(colWidthOrNone.len());
        for (usize i = 0; i < grid.size.x; ++i) {
            auto finalColWidth = colWidthOrNone[i].unwrapOr(0_px);
            colWidth.pushBack(finalColWidth);
        }
    }

    // MARK: Auto Table Layout -------------------------------------------------
    // https://www.w3.org/TR/CSS22/tables.html#auto-table-layout

    void computeAutoColWidths(Tree &tree, Input &input) {
        // FIXME: This is a rough approximation of the algorithm.
        //        We need to distinguish between percentage-based and fixed lengths:
        //         - Percentage-based sizes are fixed and cannot have extra space distributed to them.
        //         - Fixed lengths can receive extra space.
        //        Currently, we lack a predicate to differentiate percentage sizes from fixed lengths.
        //
        //        Additionally, to fully implement the spec at:
        //        https://www.w3.org/TR/css-tables-3/#intrinsic-percentage-width-of-a-column-based-on-cells-of-span-up-to-1
        //        We will need a way to retrieve the percentage value, which is also not yet implemented.

        Px capmin{0};
        for (auto i : captionsIdxs) {
            auto captionOutput = layout(
                tree,
                wrapperBox.children()[i],
                Input{
                    .commit = Commit::NO,
                    .intrinsic = IntrinsicSize::MIN_CONTENT,
                }
            );
            capmin = max(capmin, captionOutput.size.x);
        }

        auto getCellMinMaxWidth = [](Tree &tree, Box &box, Input &input, TableCell &cell) -> Pair<Px> {
            auto cellMinOutput = layout(
                tree,
                *cell.box,
                Input{
                    .commit = Commit::NO,
                    .intrinsic = IntrinsicSize::MIN_CONTENT,
                }
            );

            auto cellMaxOutput = layout(
                tree,
                *cell.box,
                Input{
                    .commit = Commit::NO,
                    .intrinsic = IntrinsicSize::MAX_CONTENT,
                }
            );

            auto cellMinWidth = cellMinOutput.size.x;
            auto cellMaxWidth = cellMaxOutput.size.x;

            if (cell.box->style->sizing->width != Size::Type::AUTO) {
                auto cellPreferredWidth = resolve(
                    tree,
                    box,
                    cell.box->style->sizing->width.value,
                    input.availableSpace.x
                );
                cellMinWidth = max(cellMinWidth, cellPreferredWidth);

                // TODO: It is not 100% from docs if we should also use 'width'
                //       (preffered width) for the maximum cell width
                cellMaxWidth = max(cellMaxWidth, cellPreferredWidth);
            }

            return {cellMinWidth, cellMaxWidth};
        };

        Vec<Px> minColWidth{};
        minColWidth.resize(grid.size.x);

        Vec<Px> maxColWidth{};
        maxColWidth.resize(grid.size.x);

        for (usize i = 0; i < grid.size.y; ++i) {
            for (usize j = 0; j < grid.size.x; ++j) {
                auto cell = grid.get(j, i);

                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                auto colSpan = cell.box->attrs.colSpan;
                if (colSpan > 1)
                    continue;

                auto [cellMinWidth, cellMaxWidth] = getCellMinMaxWidth(tree, *cell.box, input, cell);

                minColWidth[j] = max(minColWidth[j], cellMinWidth);
                maxColWidth[j] = max(maxColWidth[j], cellMaxWidth);
            }
        }

        Opt<Px> tableComputedWidth;
        if (tableBox.style->sizing->width != Size::AUTO) {
            tableComputedWidth = resolve(tree, tableBox, tableBox.style->sizing->width.value, input.availableSpace.x);
        }

        for (auto &[start, end, el] : cols) {
            auto width = el.style->sizing->width;

            // FIXME: docs are not clear on what to do for columns with AUTO width
            if (width == Size::AUTO)
                continue;

            auto widthValue = resolve(tree, el, width.value, tableComputedWidth.unwrapOr(0_px));

            for (usize x = start; x <= end; ++x) {
                minColWidth[x] = max(minColWidth[x], widthValue);
                maxColWidth[x] = max(maxColWidth[x], widthValue);
            }
        }

        for (usize i = 0; i < grid.size.y; ++i) {
            for (usize j = 0; j < grid.size.x; ++j) {
                auto cell = grid.get(j, i);

                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                auto colSpan = cell.box->attrs.span;
                if (colSpan <= 1)
                    continue;

                auto [cellMinWidth, cellMaxWidth] = getCellMinMaxWidth(tree, *cell.box, input, cell);

                Px currSumMinColWidth{0}, currSumMaxColWidth{0};
                for (usize k = 0; k < colSpan; ++k) {
                    currSumMinColWidth += minColWidth[j + k];
                    currSumMaxColWidth += maxColWidth[j + k];
                }

                if (cellMinWidth > currSumMinColWidth) {
                    auto cellMinWidthContribution = (cellMinWidth - currSumMinColWidth) / Px{colSpan};
                    for (usize k = 0; k < colSpan; ++k) {
                        minColWidth[j + k] += cellMinWidthContribution;
                    }
                }

                if (cellMaxWidth > currSumMaxColWidth) {
                    auto cellMaxWidthContribution = (cellMaxWidth - currSumMaxColWidth) / Px{colSpan};
                    for (usize k = 0; k < colSpan; ++k) {
                        maxColWidth[j + k] += cellMaxWidthContribution;
                    }
                }
            }
        }

        for (auto &group : colGroups) {

            auto columnGroupWidth = group.el.style->sizing->width;
            if (columnGroupWidth == Size::AUTO)
                continue;

            Px currSumOfGroupWidth{0};
            for (usize x = group.start; x <= group.end; ++x) {
                currSumOfGroupWidth += minColWidth[x];
            }

            auto columnGroupWidthValue = resolve(tree, group.el, columnGroupWidth.value, input.availableSpace.x);
            if (currSumOfGroupWidth >= columnGroupWidthValue)
                continue;

            Px toDistribute = (columnGroupWidthValue - currSumOfGroupWidth) / Px{group.end - group.start + 1};
            for (usize x = group.start; x <= group.end; ++x) {
                minColWidth[x] += toDistribute;
            }
        }

        Px sumMinColWidths{0}, sumMaxColWidths{0};
        for (auto x : minColWidth)
            sumMinColWidths += x;
        for (auto x : maxColWidth)
            sumMaxColWidths += x;

        // TODO: should minColWidth or maxColWidth be forcelly used if input is MIN_CONTENT or MAX_CONTENT respectivelly?
        if (tableBox.style->sizing->width != Size::AUTO) {
            // TODO: how to resolve percentage if case of table width?
            tableUsedWidth = max(capmin, max(tableComputedWidth.unwrap(), sumMinColWidths));

            // If the used width is greater than MIN, the extra width should be distributed over the columns.
            // NOTE: A bit obvious, but assuming that specs refers to MIN columns above
            if (sumMinColWidths < tableUsedWidth) {
                auto toDistribute = tableUsedWidth - sumMinColWidths;
                for (auto &w : minColWidth)
                    w += (toDistribute * w) / sumMinColWidths;
            }

            colWidth = minColWidth;
            return;
        }

        // TODO: Specs doesnt say if we should distribute extra width over columns;
        //       also would it be over min or max columns?
        if (min(sumMaxColWidths, capmin) < input.containingBlock.x) {
            colWidth = maxColWidth;
            tableUsedWidth = max(sumMaxColWidths, capmin);
        } else {
            colWidth = minColWidth;
            tableUsedWidth = max(sumMinColWidths, capmin);
        }
    }

    // https://www.w3.org/TR/CSS22/tables.html#height-layout
    Vec<Px> rowHeight;

    void computeRowHeights(Tree &tree) {
        // NOTE: CSS 2.2 does not define how the height of table cells and
        //       table rows is calculated when their height is
        //       specified using percentage values.
        //       (See https://www.w3.org/TR/CSS22/tables.html#height-layout)
        //
        //       If definite, percentages being considered 0px
        //       (See https://www.w3.org/TR/css-tables-3/#computing-the-table-height)

        rowHeight.resize(grid.size.y);

        for (auto &row : rows) {
            auto height = row.el.style->sizing->height;
            if (height == Size::AUTO)
                continue;

            for (usize y = row.start; y <= row.end; ++y) {
                rowHeight[y] = resolve(tree, row.el, height.value, 0_px);
            }
        }

        for (usize i = 0; i < grid.size.y; ++i) {
            for (usize j = 0; j < grid.size.x; ++j) {
                auto cell = grid.get(j, i);

                if (not(cell.anchorIdx == Math::Vec2u{j, i}))
                    continue;

                // [A] CSS 2.2 does not specify how cells that span more than one row affect row height calculations except
                // that the sum of the row heights involved must be great enough to encompass the cell spanning the rows.

                auto rowSpan = cell.box->attrs.rowSpan;
                if (cell.box->style->sizing->height != Size::AUTO) {
                    auto computedHeight = resolve(
                        tree,
                        *cell.box,
                        cell.box->style->sizing->height.value,
                        0_px
                    );

                    for (usize k = 0; k < rowSpan; k++) {
                        rowHeight[i + k] = max(rowHeight[i + k], Px{computedHeight / Px{rowSpan}});
                    }
                }

                auto cellOutput = layout(
                    tree,
                    *cell.box,
                    {
                        .commit = Commit::NO,
                        .knownSize = {
                            colWidth[j],
                            NONE,
                        },
                    }
                );

                for (usize k = 0; k < rowSpan; k++) {
                    rowHeight[i + k] = max(rowHeight[i + k], Px{cellOutput.size.y / Px{rowSpan}});
                }
            }
        }

        for (usize i = 0; i < grid.size.y; ++i) {
            Px rowBorderHeight{0};
            for (usize j = 0; j < grid.size.x; ++j) {
                auto cellVertBorder = bordersGrid.get(i, j).vertical();
                rowBorderHeight = max(rowBorderHeight, cellVertBorder);
            }
            rowHeight[i] += rowBorderHeight;
        }
    }

    struct AxisHelper {
        Opt<usize> groupIdx = NONE;
        Opt<usize> axisIdx = NONE;
    };

    Vec<AxisHelper> buildAxisHelper(Vec<TableAxis> const &axes, Vec<TableGroup> const &groups, usize len) {
        Vec<AxisHelper> helper{Buf<AxisHelper>::init(len)};
        for (usize groupIdx = 0; groupIdx < groups.len(); groupIdx++) {
            for (usize i = groups[groupIdx].start; i <= groups[groupIdx].end; ++i)
                helper[i].groupIdx = groupIdx;
        }
        for (usize axisIdx = 0; axisIdx < axes.len(); axisIdx++) {
            for (usize i = axes[axisIdx].start; i <= axes[axisIdx].end; ++i)
                helper[i].axisIdx = axisIdx;
        }
        return helper;
    };

    Vec2Px tableBoxSize;
    Vec<AxisHelper> rowHelper, colHelper;

    void build(Tree &tree, Input input) {
        buildHTMLTable();
        buildBordersGrid(tree);

        rowHelper = buildAxisHelper(rows, rowGroups, grid.size.y);
        colHelper = buildAxisHelper(cols, colGroups, grid.size.x);

        // NOTE: When "table-layout: fixed" is set but "width: auto", the specs suggest
        //       that the UA can use the fixed layout after computing the width
        //      (see https://www.w3.org/TR/CSS22/visudet.html#blockwidth).
        //
        //      However, Chrome does not implement this exception, and we are not implementing it either.
        bool shouldRunAutoAlgorithm =
            tableBox.style->table->tableLayout == TableLayout::AUTO or
            tableBox.style->sizing->width == Size::AUTO;

        if (shouldRunAutoAlgorithm)
            computeAutoColWidths(tree, input);
        else
            computeFixedColWidths(tree, input);

        computeRowHeights(tree);

        tableBoxSize = Vec2Px{
            iter(colWidth).sum() + spacing.x * Px{grid.size.x + 1},
            iter(rowHeight).sum() + spacing.y * Px{grid.size.y + 1},
        };
    }

    void runTableBox(Tree &tree, Input input, Px &currPositionY) {
        PrefixSum<Px> colWidthPref{colWidth}, rowHeightPref{rowHeight};
        Px currPositionX{input.position.x};

        // table box
        layout(
            tree,
            tableBox,
            {
                .commit = Commit::YES,
                .knownSize = {
                    tableBoxSize.x + boxBorder.horizontal(),
                    tableBoxSize.y + boxBorder.vertical(),
                },
                .position = {currPositionX, currPositionY},
            }
        );

        currPositionX += boxBorder.start + spacing.x;
        currPositionY += boxBorder.top + spacing.y;
        // cells
        for (usize i = 0; i < grid.size.y; currPositionY += rowHeight[i] + spacing.y, i++) {
            Px innnerCurrPositionX = Px{currPositionX};
            for (usize j = 0; j < grid.size.x; innnerCurrPositionX += colWidth[j] + spacing.x, j++) {
                auto cell = grid.get(j, i);

                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                auto colSpan = cell.box->attrs.colSpan;
                auto rowSpan = cell.box->attrs.rowSpan;

                // TODO: In CSS 2.2, the height of a cell box is the minimum
                //       height required by the content.
                //       The table cell's 'height' property can influence
                //       the height of the row (see above), but it does not
                //       increase the height of the cell box.
                //
                //       (See https://www.w3.org/TR/CSS22/tables.html#height-layout)
                layout(
                    tree,
                    *cell.box,
                    {
                        .commit = Commit::YES,
                        .knownSize = {
                            colWidthPref.query(j, j + colSpan - 1) + spacing.x * Px{colSpan - 1},
                            rowHeightPref.query(i, i + rowSpan - 1) + spacing.y * Px{rowSpan - 1}
                        },
                        .position = {innnerCurrPositionX, currPositionY},
                    }
                );
            };
        }
    }

    void runCaptions(Tree &tree, Input input, Px tableUsedWidth, Px &currPositionY, Px &captionsHeight) {
        for (auto i : captionsIdxs) {
            auto cellOutput = layout(
                tree,
                wrapperBox.children()[i],
                {
                    .commit = input.commit,
                    .knownSize = {tableUsedWidth, NONE},
                    .position = {input.position.x, currPositionY},
                }
            );
            captionsHeight += cellOutput.size.y;
            currPositionY += captionsHeight;
        }
    }

    Output run(Tree &tree, Input input) {
        Px currPositionY{input.position.y}, captionsHeight{0};
        if (tableBox.style->table->captionSide == CaptionSide::TOP) {
            runCaptions(tree, input, tableUsedWidth, currPositionY, captionsHeight);
        }

        if (input.commit == Commit::YES) {
            runTableBox(tree, input, currPositionY);
        }

        if (tableBox.style->table->captionSide == CaptionSide::BOTTOM) {
            runCaptions(tree, input, tableUsedWidth, currPositionY, captionsHeight);
        }

        return Output::fromSize({
            tableUsedWidth + boxBorder.horizontal(),
            tableBoxSize.y + captionsHeight + boxBorder.vertical(),
        });
    }
};

Output tableLayout(Tree &tree, Box &wrapper, Input input) {
    // TODO: - vertical and horizontal alignment
    //       - borders collapse

    TableFormatingContext table(tree, wrapper);
    table.build(tree, input);
    return table.run(tree, input);
}

} // namespace Vaev::Layout
