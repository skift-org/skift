#pragma once

#include <karm-layout/align.h>
#include <karm-layout/flow.h>

#include "group.h"
#include "proxy.h"

namespace Karm::Ui {

struct GridUnit {
    enum _Unit {
        AUTO,
        FIXED,
        GROW,
    };

    _Unit unit;
    int value;

    static GridUnit auto_() {
        return {AUTO, 0};
    }

    static GridUnit fixed(int value) {
        return GridUnit{FIXED, value};
    }

    static GridUnit grow(int value = 1) {
        return GridUnit{GROW, value};
    }

    GridUnit(_Unit unit, int value) : unit(unit), value(value) {}

    Vec<GridUnit> repeated(size_t count) {
        Vec<GridUnit> units{};
        while (count--)
            units.pushBack(*this);
        return units;
    }
};

struct Cell : public Proxy<Cell> {
    Math::Vec2i _start{};
    Math::Vec2i _end{};

    Cell(Math::Vec2i start, Math::Vec2i end, Child child)
        : Proxy(child), _start(start), _end(end) {}

    Math::Vec2i start() const {
        return _start;
    }

    Math::Vec2i end() const {
        return _end;
    }
};

static inline Child cell(Math::Vec2i pos, Child child) {
    return makeStrong<Cell>(pos, pos, child);
}

static inline Child cell(Math::Vec2i start, Math::Vec2i end, Child child) {
    return makeStrong<Cell>(start, end, child);
}

struct GridStyle {
    Vec<GridUnit> rows;
    Vec<GridUnit> columns;
    Math::Vec2i gaps;
    Layout::Flow flow = Layout::Flow::LEFT_TO_RIGHT;
    Layout::Align align = Layout::Align::FILL;
};

struct GridLayout : public Group<GridLayout> {
    struct _Dim {
        int start;
        int size;
        int end() const {
            return start + size;
        }
    };

    GridStyle _style;
    Vec<_Dim> _rows;
    Vec<_Dim> _columns;

    GridLayout(GridStyle style, Children children)
        : Group(children), _style(style) {}

    int computeGrowUnitRows(Math::Recti r) {
        int total = 0;
        int grows = 0;

        for (auto &row : _style.rows) {
            if (row.unit == GridUnit::GROW) {
                grows += row.value;
            } else {
                total += row.value;
            }
        }

        int all = _style.flow.getHeight(r) - _style.gaps.y * (max(1uz, _style.rows.len()) - 1);
        int growTotal = max(0, all - total);
        return (growTotal) / max(1, grows);
    }

    int computeGrowUnitColumns(Math::Recti r) {
        int total = 0;
        int grows = 0;

        for (auto &column : _style.columns) {
            if (column.unit == GridUnit::GROW) {
                grows += column.value;
            } else {
                total += column.value;
            }
        }

        int all = _style.flow.getWidth(r) - _style.gaps.x * (max(1uz, _style.columns.len()) - 1);
        int growTotal = max(0, all - total);
        return (growTotal) / max(1, grows);
    }

    void layout(Math::Recti r) override {
        _bound = r;

        // compute the dimensions of the grid
        _rows.clear();
        _columns.clear();

        int growUnitRows = computeGrowUnitRows(r);
        int growUnitColumns = computeGrowUnitColumns(r);

        int row = _style.flow.getTop(r);
        int column = _style.flow.getStart(r);

        for (auto &r : _style.rows) {
            if (r.unit == GridUnit::GROW) {
                _rows.pushBack({_Dim{row, growUnitRows * r.value}});
                row += growUnitRows * r.value;
            } else {
                _rows.pushBack({_Dim{row, r.value}});
                row += r.value;
            }

            row += _style.gaps.y;
        }

        for (auto &c : _style.columns) {
            if (c.unit == GridUnit::GROW) {
                _columns.pushBack({_Dim{column, growUnitColumns * c.value}});
                column += growUnitColumns * c.value;
            } else {
                _columns.pushBack({_Dim{column, c.value}});
                column += c.value;
            }

            column += _style.gaps.x;
        }

        // layout the children
        int index = 0;
        for (auto &child : children()) {
            if (child.is<Cell>()) {
                auto &cell = child.unwrap<Cell>();

                auto start = cell.start();
                auto end = cell.end();

                auto startRow = _rows[start.y];
                auto startColumn = _columns[start.x];

                auto endRow = _rows[end.y];
                auto endColumn = _columns[end.x];

                auto childRect = Math::Recti{
                    startColumn.start,
                    startRow.start,
                    endColumn.end() - startColumn.start,
                    endRow.end() - startRow.start,
                };

                child->layout(childRect);
                index = end.y * _columns.len() + end.x;
            } else {
                auto row = index / _columns.len();
                auto column = index % _columns.len();

                auto startRow = _rows[row];
                auto startColumn = _columns[column];

                auto childRect = Math::Recti{
                    startColumn.start,
                    startRow.start,
                    startColumn.size,
                    startRow.size,
                };

                child->layout(childRect);
            }
            index++;
        }
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        int growUnitRows = computeGrowUnitRows(Math::Recti{0, s});
        int growUnitColumns = computeGrowUnitColumns(Math::Recti{0, s});

        int row = 0;
        bool rowGrow = false;

        int column = 0;
        bool columnGrow = false;

        for (auto &r : _style.rows) {
            if (r.unit == GridUnit::GROW) {
                row += growUnitRows * r.value;
                rowGrow = true;
            } else {
                row += r.value;
            }
        }

        for (auto &c : _style.columns) {
            if (c.unit == GridUnit::GROW) {
                column += growUnitColumns * c.value;
                columnGrow = true;
            } else {
                column += c.value;
            }
        }

        if (rowGrow && hint == Layout::Hint::MAX) {
            row = max(_style.flow.getY(s), row);
        }

        if (columnGrow && hint == Layout::Hint::MAX) {
            column = max(_style.flow.getX(s), column);
        }

        return Math::Vec2i{column, row};
    }
};

static inline Child grid(GridStyle style, Children children) {
    return makeStrong<GridLayout>(style, children);
}

static inline Child grid(GridStyle style, Meta::Same<Child> auto... children) {
    return makeStrong<GridLayout>(style, Children{children...});
}

} // namespace Karm::Ui
