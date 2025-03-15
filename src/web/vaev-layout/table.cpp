module;

#include <karm-logger/logger.h>
#include <karm-math/au.h>
#include <vaev-base/break.h>
#include <vaev-base/display.h>
#include <vaev-base/sizing.h>
#include <vaev-base/table.h>

export module Vaev.Layout:table;

import :layout;
import :values;

namespace Vaev::Layout {

void advanceUntil(MutCursor<Box>& cursor, auto pred) {
    while (not cursor.ended() and not pred(cursor->style->display))
        cursor.next();
}

struct TableCell {
    Math::Vec2u anchorIdx = {};
    MutCursor<Box> box = nullptr;

    static TableCell const EMPTY;

    bool operator==(TableCell const& c) const {
        return box == c.box and anchorIdx == c.anchorIdx;
    }

    bool isOccupied() const {
        return box != nullptr;
    }
};

struct TableAxis {
    usize start, end;
    Box& el;
};

struct TableGroup {
    usize start, end;
    Box& el;
};

struct TableGrid {
    Vec<Vec<TableCell>> rows;
    Math::Vec2u size = {0, 0};

    void increaseWidth(usize span = 1) {
        for (auto& row : rows) {
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

    TableCell& get(usize x, usize y) {
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

    Vec<Au> pref = {};

    PrefixSum(Vec<Au> v = {}) : pref(v) {
        for (usize i = 1; i < pref.len(); ++i)
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

struct TableFormatingContext : public FormatingContext {
    TableGrid grid;

    Vec<TableAxis> cols;
    Vec<TableAxis> rows;
    Vec<TableGroup> rowGroups;
    Vec<TableGroup> colGroups;

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
    usize numOfHeaderRows = 0;
    usize numOfFooterRows = 0;

    struct BorderGrid {
        usize width = 0;
        Vec<InsetsAu> borders;

        InsetsAu& get(usize i, usize j) {
            return borders[i * width + j];
        }
    };

    BorderGrid bordersGrid;

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-growing-downward-growing-cells
    void growDownwardGrowingCells() {
        for (auto& [cellIdx, cellx, width] : downwardsGrowingCells) {
            for (usize x = cellx; x < cellx + width; x++)
                grid.set(x, current.y, grid.get(cellIdx.x, cellIdx.y));
        }
    }

    // https://html.spec.whatwg.org/multipage/tables.html#algorithm-for-processing-rows
    void processRows(Box& tableRowElement) {
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
    void processRowGroup(Box& rowGroupElement) {
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

    Opt<usize> findFirstHeader(Box& box) {
        auto tableBoxChildren = box.children();
        MutCursor<Box> tableBoxCursor{tableBoxChildren};

        advanceUntil(tableBoxCursor, [](Display d) {
            return d == Display::TABLE_HEADER_GROUP;
        });

        if (tableBoxCursor.ended())
            return NONE;

        return tableBoxCursor - tableBoxChildren.begin();
    }

    // https://html.spec.whatwg.org/multipage/tables.html#forming-a-table
    void buildHTMLTable(Box& box) {
        auto indexOfHeaderGroup = findFirstHeader(box);

        auto tableBoxChildren = box.children();
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
            processRowGroup(box.children()[indexOfHeaderGroup.unwrap()]);
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

            if (indexOfHeaderGroup == (usize)(tableBoxCursor - tableBoxChildren.begin())) {
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

    void buildBordersGrid(Tree& tree) {
        bordersGrid.borders.clear();
        bordersGrid.borders.resize(grid.size.x * grid.size.y);

        bordersGrid.width = grid.size.x;

        for (usize i = 0; i < grid.size.y; ++i) {
            for (usize j = 0; j < grid.size.x; ++j) {
                auto& cell = grid.get(j, i);
                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                usize rowSpan = cell.box->attrs.rowSpan;
                usize colSpan = cell.box->attrs.colSpan;

                auto cellBorder = computeBorders(tree, *cell.box);

                // Top and bottom borders
                for (usize k = 0; k < colSpan; ++k) {
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
    Tuple<Vec<Au>, Au> getColumnBorders() {
        // NOTE: Borders are only defined for cells and Table Box
        //       Rows, columns, row groups, and column groups cannot have borders
        //       (i.e., user agents must ignore the border properties for those elements).

        Vec<Au> borders;
        Au sumBorders{0};

        for (usize j = 0; j < grid.size.x; ++j) {
            Au columnBorder{0};

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

    Vec<Au> colWidth;
    Au tableUsedWidth;

    // MARK: Fixed Table Layout ------------------------------------------------------------------------
    // https://www.w3.org/TR/CSS22/tables.html#fixed-table-layout

    void computeFixedColWidths(Tree& tree, Box& box, Au availableXSpace) {
        // NOTE: Percentages for 'width' and 'height' on the table (box)
        //       are calculated relative to the containing block of the
        //       table wrapper box, not the table wrapper box itself.
        //       (See https://www.w3.org/TR/CSS22/tables.html#model)

        // NOTE: The table does not automatically expand to fill its containing block.
        //       (https://www.w3.org/TR/CSS22/tables.html#width-layout)

        if (not(box.style->sizing->width.is<Keywords::Auto>() or box.style->sizing->width.is<CalcValue<PercentOr<Length>>>()))
            logWarn("width can't be anything other than 'auto' or a length in a table context");

        tableUsedWidth =
            not box.style->sizing->width.is<CalcValue<PercentOr<Length>>>()
                ? 0_au // AUTO case
                : resolve(tree, box, box.style->sizing->width.unwrap<CalcValue<PercentOr<Length>>>(), availableXSpace) -
                      boxBorder.horizontal(); // NOTE: maybe remove this after borderbox param is clearer

        auto [columnBorders, sumBorders] = getColumnBorders();

        Au fixedWidthToAccount = Au{grid.size.x + 1} * spacing.x;

        Vec<Opt<Au>> colWidthOrNone{};
        colWidthOrNone.resize(grid.size.x);
        for (auto& col : cols) {

            auto const& width = col.el.style->sizing->width;

            if (not(width.is<Keywords::Auto>() or width.is<CalcValue<PercentOr<Length>>>()))
                logWarn("width can't be anything other than 'auto' or a length in a table context");

            if (auto widthCalc = width.is<CalcValue<PercentOr<Length>>>()) {
                for (usize x = col.start; x <= col.end; ++x) {
                    colWidthOrNone[x] = resolve(tree, col.el, *widthCalc, tableUsedWidth);
                }
            }
            // AUTO case: do nothing
        }

        // Using first row cells to define columns widths

        usize x = 0;
        while (x < grid.size.x) {
            auto cell = grid.get(x, 0);

            auto cellBoxWidthCalc = cell.box->style->sizing->width.is<CalcValue<PercentOr<Length>>>();

            if (not(cell.box->style->sizing->width.is<Keywords::Auto>() or cellBoxWidthCalc))
                logWarn("width can't be anything other than 'auto' or a length in a table context");

            // AUTO case
            if (not cellBoxWidthCalc) {
                x++;
                continue;
            }

            if (cell.anchorIdx != Math::Vec2u{x, 0})
                continue;

            auto cellWidth = resolve(tree, *cell.box, *cellBoxWidthCalc, tableUsedWidth);
            auto colSpan = cell.box->attrs.colSpan;

            for (usize j = 0; j < colSpan; ++j, x++) {
                // FIXME: Not overriding values already computed,
                //        but should we subtract the already computed from
                //        cellWidth before division?
                if (colWidthOrNone[x] == NONE)
                    colWidthOrNone[x] = cellWidth / Au{colSpan};
            }
        }

        Au sumColsWidths{0};
        usize emptyCols{0};
        for (usize i = 0; i < grid.size.x; ++i) {
            if (colWidthOrNone[i])
                sumColsWidths += colWidthOrNone[i].unwrap() + columnBorders[i];
            else
                emptyCols++;
        }

        if (emptyCols > 0) {
            if (sumColsWidths < tableUsedWidth - fixedWidthToAccount) {
                Au toDistribute = (tableUsedWidth - fixedWidthToAccount - sumColsWidths) / Au{emptyCols};
                for (auto& w : colWidthOrNone)
                    if (w == NONE)
                        w = toDistribute;
            }
        } else if (sumColsWidths < tableUsedWidth - fixedWidthToAccount) {
            Au toDistribute = (tableUsedWidth - fixedWidthToAccount - sumColsWidths);
            for (auto& w : colWidthOrNone) {
                w = w.unwrap() + (toDistribute * w.unwrap()) / sumColsWidths;
            }
        }

        colWidth.ensure(colWidthOrNone.len());
        for (usize i = 0; i < grid.size.x; ++i) {
            auto finalColWidth = colWidthOrNone[i].unwrapOr(0_au);
            colWidth.pushBack(finalColWidth);
        }
    }

    // MARK: Auto Table Layout -------------------------------------------------
    Pair<Au> getCellMinMaxAutoWidth(Tree& tree, Box& box, TableCell& cell, Au tableComputedWidth) {
        auto cellMinOutput = computeIntrinsicSize(
            tree,
            box,
            IntrinsicSize::MIN_CONTENT,
            {} // FIXME
        );

        auto cellMaxOutput = computeIntrinsicSize(
            tree,
            box,
            IntrinsicSize::MAX_CONTENT,
            {} // FIXME
        );

        auto cellMinWidth = cellMinOutput.x;
        auto cellMaxWidth = cellMaxOutput.x;

        if (not(cell.box->style->sizing->width.is<Keywords::Auto>() or cell.box->style->sizing->width.is<CalcValue<PercentOr<Length>>>()))
            logWarn("width can't be anything other than 'auto' or a length in a table context");

        if (auto cellBoxWidthCalc = cell.box->style->sizing->width.is<CalcValue<PercentOr<Length>>>()) {
            auto cellPreferredWidth = resolve(
                tree,
                box,
                *cellBoxWidthCalc,
                tableComputedWidth
            );
            cellMinWidth = max(cellMinWidth, cellPreferredWidth);

            // TODO: It is not 100% from docs if we should also use 'width'
            //       (preferred width) for the maximum cell width
            cellMaxWidth = max(cellMaxWidth, cellPreferredWidth);
        }

        return {cellMinWidth, cellMaxWidth};
    };

    void computeAutoWidthOfCellsSpan1(Tree& tree, Vec<Au>& minColWidth, Vec<Au>& maxColWidth, Au tableWidth) {
        for (usize i = 0; i < grid.size.y; ++i) {
            for (usize j = 0; j < grid.size.x; ++j) {
                auto cell = grid.get(j, i);

                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                auto colSpan = cell.box->attrs.colSpan;
                if (colSpan > 1)
                    continue;

                auto [cellMinWidth, cellMaxWidth] = getCellMinMaxAutoWidth(tree, *cell.box, cell, tableWidth);

                minColWidth[j] = max(minColWidth[j], cellMinWidth);
                maxColWidth[j] = max(maxColWidth[j], cellMaxWidth);
            }
        }
    }

    void computeAutoWidthOfCellsSpanN(Tree& tree, Vec<Au>& minColWidth, Vec<Au>& maxColWidth, Au tableWidth) {
        for (usize i = 0; i < grid.size.y; ++i) {
            for (usize j = 0; j < grid.size.x; ++j) {
                auto cell = grid.get(j, i);

                if (cell.anchorIdx != Math::Vec2u{j, i})
                    continue;

                auto colSpan = cell.box->attrs.span;
                if (colSpan <= 1)
                    continue;

                auto [cellMinWidth, cellMaxWidth] = getCellMinMaxAutoWidth(tree, *cell.box, cell, tableWidth);

                Au currSumMinColWidth{0};
                Au currSumMaxColWidth{0};

                for (usize k = 0; k < colSpan; ++k) {
                    currSumMinColWidth += minColWidth[j + k];
                    currSumMaxColWidth += maxColWidth[j + k];
                }

                if (cellMinWidth > currSumMinColWidth) {
                    auto cellMinWidthContribution = (cellMinWidth - currSumMinColWidth) / Au{colSpan};
                    for (usize k = 0; k < colSpan; ++k) {
                        minColWidth[j + k] += cellMinWidthContribution;
                    }
                }

                if (cellMaxWidth > currSumMaxColWidth) {
                    auto cellMaxWidthContribution = (cellMaxWidth - currSumMaxColWidth) / Au{colSpan};
                    for (usize k = 0; k < colSpan; ++k) {
                        maxColWidth[j + k] += cellMaxWidthContribution;
                    }
                }
            }
        }
    }

    void computeAutoWidthOfColGroups(Tree& tree, Vec<Au>& minColWidth, Au tableWidth) {
        for (auto& group : colGroups) {

            auto columnGroupWidth = group.el.style->sizing->width;
            auto columnGroupWidthCalc = columnGroupWidth.is<CalcValue<PercentOr<Length>>>();

            if (not(columnGroupWidth.is<Keywords::Auto>() or columnGroupWidthCalc))
                logWarn("width can't be anything other than 'auto' or a length in a table context");

            // AUTO case
            if (not columnGroupWidthCalc)
                continue;

            Au currSumOfGroupWidth{0};
            for (usize x = group.start; x <= group.end; ++x) {
                currSumOfGroupWidth += minColWidth[x];
            }

            auto columnGroupWidthValue = resolve(tree, group.el, *columnGroupWidthCalc, tableWidth);
            if (currSumOfGroupWidth >= columnGroupWidthValue)
                continue;

            Au toDistribute = (columnGroupWidthValue - currSumOfGroupWidth) / Au{group.end - group.start + 1};
            for (usize x = group.start; x <= group.end; ++x) {
                minColWidth[x] += toDistribute;
            }
        }
    }

    void computeAutoWidthOfCols(Tree& tree, Vec<Au>& minColWidth, Vec<Au>& maxColWidth, Au tableWidth) {
        for (auto& [start, end, el] : cols) {
            auto width = el.style->sizing->width;
            auto widthCalc = width.is<CalcValue<PercentOr<Length>>>();

            if (not(width.is<Keywords::Auto>() or widthCalc))
                logWarn("width can't be anything other than 'auto' or a length in a table context");

            // FIXME: docs are not clear on what to do for columns with AUTO width
            if (not widthCalc) // AUTO case
                continue;

            auto widthValue = resolve(tree, el, *widthCalc, tableWidth);

            for (usize x = start; x <= end; ++x) {
                minColWidth[x] = max(minColWidth[x], widthValue);
                maxColWidth[x] = max(maxColWidth[x], widthValue);
            }
        }
    }

    Pair<Vec<Au>> computeMinMaxAutoWidths(Tree& tree, usize size, Au tableWidth) {
        Vec<Au> minColWidth{};
        minColWidth.resize(size);

        Vec<Au> maxColWidth{};
        maxColWidth.resize(size);

        computeAutoWidthOfCellsSpan1(tree, minColWidth, maxColWidth, tableWidth);
        computeAutoWidthOfCols(tree, minColWidth, maxColWidth, tableWidth);
        computeAutoWidthOfCellsSpanN(tree, minColWidth, maxColWidth, tableWidth);
        computeAutoWidthOfColGroups(tree, minColWidth, tableWidth);

        return {minColWidth, maxColWidth};
    }

    // https://www.w3.org/TR/CSS22/tables.html#auto-table-layout

    void computeAutoColWidths(Tree& tree, Box& box, Au capmin, Au availableXSpace, Au containingBlockX) {
        // FIXME: This is a rough approximation of the algorithm.
        //        We need to distinguish between percentage-based and fixed lengths:
        //         - Percentage-based sizes are fixed and cannot have extra space distributed to them.
        //         - Fixed lengths can receive extra space.
        //        Currently, we lack a predicate to differentiate percentage sizes from fixed lengths.
        //
        //        Additionally, to fully implement the spec at:
        //        https://www.w3.org/TR/css-tables-3/#intrinsic-percentage-width-of-a-column-based-on-cells-of-span-up-to-1
        //        We will need a way to retrieve the percentage value, which is also not yet implemented.

        if (auto boxWidthCalc = box.style->sizing->width.is<CalcValue<PercentOr<Length>>>()) {
            auto [minWithoutPerc, maxWithoutPerc] = computeMinMaxAutoWidths(tree, grid.size.x, 0_au);

            Au tableComputedWidth = resolve(tree, box, *boxWidthCalc, availableXSpace);
            tableUsedWidth = max(capmin, tableComputedWidth);

            auto sumMinWithoutPerc = iter(minWithoutPerc).sum();
            if (sumMinWithoutPerc > tableUsedWidth) {
                tableUsedWidth = sumMinWithoutPerc;
                colWidth = minWithoutPerc;
                return;
            }

            auto [minWithPerc, maxWithPerc] = computeMinMaxAutoWidths(tree, grid.size.x, tableComputedWidth);

            auto sumMaxWithoutPerc = iter(maxWithoutPerc).sum();
            Vec<Au>& distWOPToUse = sumMaxWithoutPerc < tableUsedWidth ? maxWithoutPerc : minWithoutPerc;
            Vec<Au>& distWPToUse = sumMaxWithoutPerc < tableUsedWidth ? maxWithPerc : minWithPerc;

            auto sumWithPerc = iter(distWPToUse).sum();
            auto sumWithoutPerc = iter(distWOPToUse).sum();

            if (sumWithPerc > tableUsedWidth) {
                Au totalDiff = sumWithPerc - sumWithoutPerc;
                Au allowedGrowth = tableUsedWidth - sumWithoutPerc;

                // it should grow only (allowedGrowth / totalDiff)
                for (usize j = 0; j < grid.size.x; ++j) {
                    if (distWPToUse[j] != distWOPToUse[j]) {
                        Au diff = distWPToUse[j] - distWOPToUse[j];
                        distWOPToUse[j] += (diff * allowedGrowth) / totalDiff;
                    }
                }
                colWidth = distWOPToUse;
            } else {
                auto toDistribute = tableUsedWidth - sumWithPerc;
                for (auto& w : distWPToUse)
                    w += (toDistribute * w) / sumWithPerc;
                colWidth = distWPToUse;
            }
        } else {
            auto [minColWidth, maxColWidth] = computeMinMaxAutoWidths(tree, grid.size.x, 0_au);
            auto sumMaxColWidths = iter(maxColWidth).sum();
            auto sumMinColWidths = iter(minColWidth).sum();

            // TODO: Specs doesnt say if we should distribute extra width over columns;
            //       also would it be over min or max columns?
            if (min(sumMaxColWidths, capmin) < containingBlockX) {
                colWidth = maxColWidth;
                tableUsedWidth = max(sumMaxColWidths, capmin);
            } else {
                colWidth = minColWidth;
                tableUsedWidth = max(sumMinColWidths, capmin);
            }
        }
    }

    // https://www.w3.org/TR/CSS22/tables.html#height-layout
    Vec<Au> rowHeight;

    void computeRowHeights(Tree& tree) {
        // NOTE: CSS 2.2 does not define how the height of table cells and
        //       table rows is calculated when their height is
        //       specified using percentage values.
        //       (See https://www.w3.org/TR/CSS22/tables.html#height-layout)
        //
        //       If definite, percentages being considered 0px
        //       (See https://www.w3.org/TR/css-tables-3/#computing-the-table-height)

        rowHeight.resize(grid.size.y);

        for (auto& row : rows) {
            auto& height = row.el.style->sizing->height;
            auto heightCalc = height.is<CalcValue<PercentOr<Length>>>();

            if (not(height.is<Keywords::Auto>() or heightCalc))
                logWarn("height can't be anything other than 'auto' or a length in a table context");

            // AUTO case
            if (not heightCalc)
                continue;

            for (usize y = row.start; y <= row.end; ++y) {
                rowHeight[y] = resolve(tree, row.el, *heightCalc, 0_au);
            }
        }

        for (usize i = 0; i < grid.size.y; ++i) {
            for (usize j = 0; j < grid.size.x; ++j) {
                auto cell = grid.get(j, i);

                if (not(cell.anchorIdx == Math::Vec2u{j, i}))
                    continue;

                // [A] CSS 2.2 does not specify how cells that span more than one row affect row height calculations except
                // that the sum of the row heights involved must be great enough to encompass the cell spanning the rows.

                if (not(cell.box->style->sizing->height.is<Keywords::Auto>() or cell.box->style->sizing->height.is<CalcValue<PercentOr<Length>>>()))
                    logWarn("height can't be anything other than 'auto' or a length in a table context");

                auto rowSpan = cell.box->attrs.rowSpan;
                if (auto cellBoxHeightCalc = cell.box->style->sizing->height.is<CalcValue<PercentOr<Length>>>()) {
                    auto computedHeight = resolve(
                        tree,
                        *cell.box,
                        *cellBoxHeightCalc,
                        0_au
                    );

                    for (usize k = 0; k < rowSpan; k++) {
                        rowHeight[i + k] = max(rowHeight[i + k], Au{computedHeight / Au{rowSpan}});
                    }
                }

                auto cellOutput = layout(
                    tree,
                    *cell.box,
                    {
                        .intrinsic = IntrinsicSize::MIN_CONTENT,
                        .knownSize = {colWidth[j], NONE},
                    }
                );

                for (usize k = 0; k < rowSpan; k++) {
                    rowHeight[i + k] = max(rowHeight[i + k], Au{cellOutput.size.y / Au{rowSpan}});
                }
            }
        }

        for (usize i = 0; i < grid.size.y; ++i) {
            Au rowBorderHeight{0};
            for (usize j = 0; j < grid.size.x; ++j) {
                auto cellVertBorder = bordersGrid.get(i, j).vertical();
                rowBorderHeight = max(rowBorderHeight, cellVertBorder);
            }
            rowHeight[i] += rowBorderHeight;
        }
    }

    struct AxisHelper { // FIXME: find me a better name pls
        Opt<usize> groupIdx = NONE;
        Opt<usize> axisIdx = NONE;
    };

    Vec<AxisHelper> buildAxisHelper(Vec<TableAxis> const& axes, Vec<TableGroup> const& groups, usize len) {
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

    InsetsAu boxBorder;
    Vec2Au spacing;
    Vec2Au tableBoxSize = {}, headerSize = {}, footerSize = {};
    Vec<AxisHelper> rowHelper, colHelper;
    PrefixSum<Au> colWidthPref, rowHeightPref;
    Math::Vec2u dataRowsInterval;
    Vec<Au> startPositionOfRow;

    struct CacheParametersFromInput {
        Au availableXSpace;
        Au containingBlockX;
        Au capmin;

        CacheParametersFromInput(Input const& i)
            : availableXSpace(i.availableSpace.x),
              containingBlockX(i.containingBlock.x),
              capmin(i.capmin.unwrap()) {}

        bool operator==(CacheParametersFromInput const& c) const = default;
    };

    void build(Tree& tree, Box& box) override {
        boxBorder = computeBorders(tree, box);
        spacing = {
            resolve(tree, box, box.style->table->spacing.horizontal),
            resolve(tree, box, box.style->table->spacing.vertical),
        };

        buildHTMLTable(box);
        buildBordersGrid(tree);

        rowHelper = buildAxisHelper(rows, rowGroups, grid.size.y);
        colHelper = buildAxisHelper(cols, colGroups, grid.size.x);

        startPositionOfRow.clear();
        startPositionOfRow.resize(grid.size.y, 0_au);

        dataRowsInterval = {numOfHeaderRows, grid.size.y - numOfFooterRows - 1};
    }

    void computeWidthAndHeight(Tree& tree, Box& box, CacheParametersFromInput& input) {
        // NOTE: When "table-layout: fixed" is set but "width: auto", the specs suggest
        //       that the UA can use the fixed layout after computing the width
        //      (see https://www.w3.org/TR/CSS22/visudet.html#blockwidth).
        //
        //      However, Chrome does not implement this exception, and we are not implementing it either.
        bool shouldRunAutoAlgorithm =
            box.style->table->tableLayout == TableLayout::AUTO or
            box.style->sizing->width.is<Keywords::Auto>();

        if (shouldRunAutoAlgorithm)
            computeAutoColWidths(
                tree, box, input.capmin,
                input.availableXSpace, input.containingBlockX
            );
        else
            computeFixedColWidths(tree, box, input.availableXSpace);

        computeRowHeights(tree);

        colWidthPref = PrefixSum<Au>{colWidth};
        rowHeightPref = PrefixSum<Au>{rowHeight};

        tableBoxSize = Vec2Au{
            iter(colWidth).sum() + spacing.x * Au{grid.size.x + 1},
            iter(rowHeight).sum() + spacing.y * Au{grid.size.y + 1},
        };

        if (numOfHeaderRows) {
            headerSize = Vec2Au{
                tableBoxSize.x,
                rowHeightPref.query(0, numOfHeaderRows - 1) + spacing.y * Au{numOfHeaderRows + 1},
            };
        }

        if (numOfFooterRows) {
            footerSize = Vec2Au{
                tableBoxSize.x,
                rowHeightPref.query(grid.size.y - numOfFooterRows, grid.size.y - 1) +
                    spacing.y * Au{numOfHeaderRows + 1},
            };
        }
    }

    Tuple<Output, Au> layoutCell(Tree& tree, Input& input, TableCell& cell, MutCursor<Box> cellBox, usize startFrag, usize i, usize j, Au currPositionX, usize breakpointIndexOffset) {

        // breakpoint traversing for a cell that started in the previous fragmentainer is not trivial
        // since it started in the previous fragmentainer, its breakpoint must be of type ADVANCE_WITH_CHILDREN and thus
        // children info will be available at startFrag
        // however, breakpoints set in the new iteration can be at i, in case of a cell with 3 or more rows (first row
        // in prev frag, second row is startFrag, and third row is i); thus, we need to detach the traversing from the
        // previous frag iteration from the current
        BreakpointTraverser breakpointsForCell;
        if (not input.breakpointTraverser.isDeactivated()) {
            // in case of headers or footers, breakpoints would have been deactivated
            if (startFrag < cell.anchorIdx.y)
                breakpointsForCell = input.breakpointTraverser.traverseInsideUsingIthChildToJthParallelFlow(i - breakpointIndexOffset, j);
            else
                breakpointsForCell = BreakpointTraverser{
                    input.breakpointTraverser.traversePrev(startFrag - breakpointIndexOffset, j),
                    input.breakpointTraverser.traverseCurr(i - breakpointIndexOffset, j),
                };
        }

        // if the box started being rendered in the previous fragment,
        // - its started position must be row starting the fragment
        // - its size cant be the one computed in the build phase, thus is NONE
        auto rowSpan = cell.box->attrs.rowSpan;
        bool boxStartedInPrevFragment = tree.fc.allowBreak() and breakpointsForCell.prevIteration;
        Au startPositionY = startPositionOfRow[boxStartedInPrevFragment ? startFrag : cell.anchorIdx.y];

        Opt<Au> verticalSize;
        if (not boxStartedInPrevFragment) {
            verticalSize =
                rowHeightPref.query(cell.anchorIdx.y, cell.anchorIdx.y + rowSpan - 1) +
                spacing.y * Au{rowSpan - 1};
        }

        // TODO: In CSS 2.2, the height of a cell box is the minimum
        //       height required by the content.
        //       The table cell's 'height' property can influence
        //       the height of the row (see above), but it does not
        //       increase the height of the cell box.
        //
        //       (See https://www.w3.org/TR/CSS22/tables.html#height-layout)
        auto colSpan = cell.box->attrs.colSpan;
        auto outputCell = layout(
            tree,
            *cell.box,
            {
                .fragment = input.fragment,
                .knownSize = {
                    colWidthPref.query(j, j + colSpan - 1) + spacing.x * Au{colSpan - 1},
                    verticalSize,
                },
                .position = {currPositionX, startPositionY},
                .breakpointTraverser = breakpointsForCell,
                .pendingVerticalSizes = input.pendingVerticalSizes,
            }
        );

        if (tree.fc.isDiscoveryMode()) {
            if (cellBox->style->break_->inside == BreakInside::AVOID) {
                outputCell.breakpoint.unwrap().withAppeal(Breakpoint::Appeal::AVOID);
            }
        }

        return {
            outputCell,
            startPositionY + outputCell.height() - startPositionOfRow[i]
        };
    }

    struct RowOutput {
        Au sizeY = 0_au;

        bool allBottomsAndCompletelyLaidOut = true;
        bool someBottomsUncompleteLaidOut = false;

        Vec<Opt<Breakpoint>> breakpoints = {};
        Vec<bool> isBottom = {};
    };

    RowOutput layoutRow(Tree& tree, Input input, usize startFrag, usize i, Vec2Au currPosition, bool isBreakpointedRow, usize breakpointIndexOffset = 0) {
        startPositionOfRow[i] = currPosition.y;

        RowOutput outputRow;
        if (tree.fc.isDiscoveryMode()) {
            outputRow.breakpoints = Buf<Opt<Breakpoint>>::init(grid.size.x, NONE);
            outputRow.isBottom = Buf<bool>::init(grid.size.x, false);
        }

        currPosition.x += spacing.x;
        for (usize j = 0; j < grid.size.x; currPosition.x += colWidth[j] + spacing.x, j++) {
            auto cell = grid.get(j, i);
            auto cellBox = grid.get(cell.anchorIdx.x, cell.anchorIdx.y).box;

            if (cell.anchorIdx.x != j)
                continue;

            bool isBottomCell = cell.anchorIdx.y + cellBox->attrs.rowSpan - 1 == i;

            if (not tree.fc.isDiscoveryMode() and not(isBottomCell or isBreakpointedRow)) {
                continue;
            }

            auto [outputCell, cellHeight] = layoutCell(tree, input, cell, cellBox, startFrag, i, j, currPosition.x, breakpointIndexOffset);

            if (tree.fc.isDiscoveryMode()) {
                if (isBottomCell)
                    outputRow.sizeY = max(outputRow.sizeY, cellHeight);

                outputRow.breakpoints[j] = outputCell.breakpoint;
                outputRow.isBottom[j] = isBottomCell;
                outputRow.allBottomsAndCompletelyLaidOut &= isBottomCell and outputCell.completelyLaidOut;
            } else {
                outputRow.sizeY = max(outputRow.sizeY, cellHeight);
            }
            outputRow.someBottomsUncompleteLaidOut |= isBottomCell and not outputCell.completelyLaidOut;
        };

        return outputRow;
    }

    // https://www.w3.org/TR/css-tables-3/#freely-fragmentable
    bool isFreelyFragmentableRow(usize i, Vec2Au fragmentainerSize) {
        /*
        NOT freely fragmentable if (AND):
        - if the cells spanning the row do not span any subsequent row
            - all cells are row span = 1
        - their height is at least twice smaller than both the fragmentainer height and width
            - height <= min(frag.height, frag.width) / 2

        it is freely fragmentable if at least one cell has row span > 1 OR height > min(frag.height, frag.width) / 2
        */

        bool isSelfContainedRow = true;
        for (usize j = 0; j < grid.size.x; ++j) {
            if (grid.get(j, i).anchorIdx != Math::Vec2u{j, i} or grid.get(j, i).box->attrs.rowSpan != 1) {
                isSelfContainedRow = false;
                break;
            }
        }

        return not isSelfContainedRow or
               rowHeight[i] * 2_au > min(fragmentainerSize.x, fragmentainerSize.y);
    }

    bool handlePossibleForcedBreakpointAfterRow(Breakpoint& currentBreakpoint, bool allBottomsAndCompletelyLaidOut, bool isLastRow, usize i) {
        if (not allBottomsAndCompletelyLaidOut or isLastRow)
            return true;

        // if row is self contained, the <tr> it belongs to has size 1
        bool forcedBreakAfterCurrRow =
            rowHelper[i].axisIdx and
            rows[rowHelper[i].axisIdx.unwrap()].el.style->break_->after == BreakBetween::PAGE;

        bool forcedBreakBeforeNextRow =
            i + 1 <= dataRowsInterval.y and
            rowHelper[i + 1].axisIdx and
            rows[rowHelper[i + 1].axisIdx.unwrap()].el.style->break_->before == BreakBetween::PAGE;

        bool limitOfCurrRowGroup = i + 1 <= dataRowsInterval.y and rowHelper[i].groupIdx != rowHelper[i + 1].groupIdx;

        bool forcedBreakAfterCurrRowGroup =
            limitOfCurrRowGroup and
            rowHelper[i].groupIdx and
            rowGroups[rowHelper[i].groupIdx.unwrap()].el.style->break_->after == BreakBetween::PAGE;

        bool forcedBreakBeforeNextRowGroup =
            limitOfCurrRowGroup and
            i + 1 <= dataRowsInterval.y and
            rowHelper[i + 1].groupIdx and
            rowGroups[rowHelper[i + 1].groupIdx.unwrap()].el.style->break_->before == BreakBetween::PAGE;

        if (forcedBreakAfterCurrRow or forcedBreakBeforeNextRow or
            forcedBreakAfterCurrRowGroup or forcedBreakBeforeNextRowGroup) {
            currentBreakpoint = Breakpoint::forced(i + 1);
            return false;
        }

        return true;
    }

    bool handleUnforcedBreakpointsInsideAndAfterRow(Box& box, Breakpoint& currentBreakpoint, RowOutput outputRow, usize i, Vec2Au fragmentainerSize) {
        bool rowIsFreelyFragmentable = isFreelyFragmentableRow(i, fragmentainerSize);

        bool avoidBreakInsideTable = box.style->break_->inside == BreakInside::AVOID;

        bool avoidBreakInsideRow =
            rowHelper[i].axisIdx and
            rows[rowHelper[i].axisIdx.unwrap()].el.style->break_->inside == BreakInside::AVOID;

        bool avoidBreakInsideRowGroup =
            rowHelper[i].groupIdx and
            rowGroups[rowHelper[i].groupIdx.unwrap()].el.style->break_->inside == BreakInside::AVOID;

        if (rowIsFreelyFragmentable) {
            // breakpoint inside of row, take in consideration ALL breakpoints
            // should stay in this row next fragmentation
            currentBreakpoint.overrideIfBetter(Breakpoint::fromChildren(outputRow.breakpoints, i + 1, avoidBreakInsideRow or avoidBreakInsideRowGroup or avoidBreakInsideTable, false));
        }

        // we need to abort layout if we cannot fit cells on their last row
        if (outputRow.someBottomsUncompleteLaidOut)
            return false;

        if (not outputRow.allBottomsAndCompletelyLaidOut) {
            // breakpoint outside of row, but taking into consideration ONLY breakpoints of cells which are not
            // in their bottom row
            // since someBottomsUncompleteLaidOut is False, all bottom cells were able to be completed and their
            // computed breakpoints can be disregarded
            for (usize j = 0; j < grid.size.x; j++) {
                if (outputRow.isBottom[j])
                    outputRow.breakpoints[j] = NONE;
            }

            currentBreakpoint.overrideIfBetter(Breakpoint::fromChildren(outputRow.breakpoints, i + 1, avoidBreakInsideRow or avoidBreakInsideRowGroup or avoidBreakInsideTable, true));

        } else {

            // no cells are being split
            currentBreakpoint.overrideIfBetter(Breakpoint::classB(i + 1, avoidBreakInsideTable));
        }

        return true;
    }

    Tuple<bool, Opt<Breakpoint>> layoutRows(Tree& tree, Box& box, Input input, usize startAt, usize stopAt, Au currPositionX, Au& currPositionY, bool shouldRepeatHeaderAndFooter) {
        bool completelyLaidOut = false;
        Opt<Breakpoint> rowBreakpoint = NONE;

        if (tree.fc.isDiscoveryMode()) {
            completelyLaidOut = true;
            rowBreakpoint = Breakpoint();

            if (shouldRepeatHeaderAndFooter)
                input = input.addPendingVerticalSize(footerSize.y);
        }

        for (usize i = startAt; i < stopAt; i++) {
            auto rowOutput = layoutRow(
                tree, input, startAt,
                i,
                Vec2Au{currPositionX, currPositionY},
                i + 1 == stopAt,
                shouldRepeatHeaderAndFooter ? dataRowsInterval.x : 0
            );

            if (tree.fc.isDiscoveryMode()) {
                if (
                    not handleUnforcedBreakpointsInsideAndAfterRow(box, rowBreakpoint.unwrap(), rowOutput, i, tree.fc.size()) or
                    not handlePossibleForcedBreakpointAfterRow(rowBreakpoint.unwrap(), rowOutput.allBottomsAndCompletelyLaidOut, (i + 1 == stopAt), i)
                ) {
                    completelyLaidOut = false;
                    break;
                }
            } else if (i == (shouldRepeatHeaderAndFooter ? dataRowsInterval.y : grid.size.y - 1)) {
                completelyLaidOut = not rowOutput.someBottomsUncompleteLaidOut;
            }

            currPositionY += rowOutput.sizeY + spacing.y;
        }
        return {completelyLaidOut, rowBreakpoint};
    }

    void layoutHeaderFooterRows(Tree& tree, Input input, usize startFrag, Au currPositionX, Au& currPositionY, usize start, usize len) {
        for (usize i = 0; i < len; i++) {
            auto _ = layoutRow(
                tree,
                input.withBreakpointTraverser(BreakpointTraverser()),
                startFrag, start + i,
                Vec2Au{currPositionX, currPositionY}, false
            );
            currPositionY += rowHeight[i] + spacing.y;
        }
    }

    Tuple<usize, usize> computeLayoutIntervals(Tree& tree, bool shouldRepeatHeaderAndFooter, usize startAtTable, Opt<usize> stopAtTable) {
        usize startAt = startAtTable + (shouldRepeatHeaderAndFooter ? dataRowsInterval.x : 0);
        usize stopAt;
        if (tree.fc.isDiscoveryMode()) {
            stopAt = shouldRepeatHeaderAndFooter
                         ? dataRowsInterval.y + 1
                         : grid.size.y;
        } else {
            stopAt = shouldRepeatHeaderAndFooter
                         ? dataRowsInterval.x + stopAtTable.unwrapOr(dataRowsInterval.y - dataRowsInterval.x + 1)
                         : stopAtTable.unwrapOr(grid.size.y);
        }

        return {startAt, stopAt};
    }

    Opt<CacheParametersFromInput> lastInput;

    Output run(Tree& tree, Box& box, Input input, usize startAtTable, Opt<usize> stopAtTable) override {
        // TODO: - vertical and horizontal alignment
        //       - borders collapse
        // TODO: in every row, at least one cell must be an anchor, or else this row is 'skipable'

        CacheParametersFromInput inputCacheParameters{input};
        if (lastInput != inputCacheParameters) {
            lastInput = inputCacheParameters;
            computeWidthAndHeight(tree, box, lastInput.unwrap());
        }

        // if shouldRepeatHeaderAndFooter, header and footer are never alone in the fragmentainer and we wont set
        // breakpoints on them;
        // otherwise, they only appear once, might be alone in the fragmentainer and can be broken into pages
        bool shouldRepeatHeaderAndFooter =
            tree.fc.allowBreak() and
            max(headerSize.y, footerSize.y) * 4_au <= tree.fc.size().y and
            headerSize.y + footerSize.y * 2_au <= tree.fc.size().y;

        Au currPositionX{input.position.x};
        Au currPositionY{input.position.y};
        Au startingPositionY = currPositionY;
        currPositionY += spacing.y;

        auto [startAt, stopAt] = computeLayoutIntervals(
            tree, shouldRepeatHeaderAndFooter, startAtTable, stopAtTable
        );

        if (shouldRepeatHeaderAndFooter)
            layoutHeaderFooterRows(
                tree, input,
                startAt,
                currPositionX, currPositionY,
                0, numOfHeaderRows
            );

        auto [completelyLaidOut, breakpoint] = layoutRows(
            tree, box, input,
            startAt, stopAt,
            currPositionX, currPositionY,
            shouldRepeatHeaderAndFooter
        );

        if (tree.fc.isDiscoveryMode() and shouldRepeatHeaderAndFooter) {
            breakpoint.unwrap().endIdx -= dataRowsInterval.x;
        }

        if (shouldRepeatHeaderAndFooter)
            layoutHeaderFooterRows(
                tree, input,
                startAt,
                currPositionX, currPositionY,
                grid.size.y - numOfFooterRows, numOfFooterRows
            );

        return Output{
            .size = {tableUsedWidth, currPositionY - startingPositionY},
            .completelyLaidOut = completelyLaidOut,
            .breakpoint = tree.fc.isDiscoveryMode() ? Opt<Breakpoint>{breakpoint} : NONE,
        };
    }
};

Rc<FormatingContext> constructTableFormatingContext(Box&) {
    return makeRc<TableFormatingContext>();
}

} // namespace Vaev::Layout
