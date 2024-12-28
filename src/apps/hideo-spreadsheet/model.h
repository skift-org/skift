#pragma once

#include <karm-base/map.h>
#include <karm-text/font.h>
#include <karm-ui/reducer.h>

namespace Hideo::Spreadsheet {

using Value = Union<None, String, f64, bool>;

struct Pos {
    usize row;
    usize col;

    auto operator<=>(Pos const &) const = default;
};

enum struct Wheight {
    NONE,
    THIN,
    NORMAL,
    THICK,
    DASHED,
    DOTTED,
};

struct Border {
    Wheight style;
    Gfx::Color color = Gfx::BLACK;
};

struct Borders {
    Border top = {};
    Border end = {};
    Border bottom = {};
    Border start = {};
};

enum struct Align {
    START,
    CENTER,
    END,
};

struct Style {
    Gfx::Color fg = Gfx::BLACK;
    Gfx::Color bg = Gfx::WHITE;
    Align halign;
    Align valign;
    Opt<Text::Font> font;
    Borders borders{};
};

struct Cell {
    Value value = NONE;
    Style style;
};

static constexpr isize CELL_WIDTH = 96;
static constexpr isize CELL_HEIGHT = 24;

struct Row {
    isize height = CELL_HEIGHT;
    isize y = 0;
};

struct Col {
    isize width = CELL_WIDTH;
    isize x = 0;
};

struct Sheet {
    static constexpr usize MAX_ROWS = Limits<u16>::MAX;
    static constexpr usize MAX_COLS = 26;

    String name;
    usize freezedRows = 0;
    usize freezedCols = 0;
    Vec<Row> rows = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}};
    Vec<Col> cols = {{}, {}, {}, {}, {}, {}, {}, {}};
    Map<Pos, Cell> cells = {};

    void recompute() {
        i32 y = 0;
        for (auto &row : rows) {
            row.y = y;
            y += row.height;
        }

        i32 x = 0;
        for (auto &col : cols) {
            col.x = x;
            x += col.width;
        }
    }

    Opt<Pos> cellAt(Math::Vec2i p) const {
        auto row = rowAt(p.y);
        auto col = colAt(p.x);
        if (row and col)
            return Pos{*row, *col};
        return NONE;
    }

    Opt<usize> rowAt(i32 y) const {
        return search(rows, [&](auto const &row) {
            if (row.y <= y and y < row.y + row.height) {
                return std::strong_ordering::equal;
            }
            return row.y <=> y;
        });
    }

    Opt<usize> colAt(i32 x) const {
        return search(cols, [&](auto const &col) {
            if (col.x <= x and x < col.x + col.width) {
                return std::strong_ordering::equal;
            }
            return col.x <=> x;
        });
    }

    usize rowLen() const {
        return rows.len();
    }

    usize colLen() const {
        return cols.len();
    }
};

struct Range {
    Pos start;
    Pos end;

    Range() = default;

    Range(Pos pos)
        : start(pos), end(pos) {}

    Range(Pos start, Pos end)
        : start(start), end(end) {}

    Range normalised() const {
        return {
            Pos{
                min(start.row, end.row),
                min(start.col, end.col),
            },
            Pos{
                max(start.row, end.row),
                max(start.col, end.col),
            },
        };
    }

    usize rows() const {
        auto n = normalised();
        return n.end.row - n.start.row + 1;
    }

    usize cols() const {
        auto n = normalised();
        return n.end.col - n.start.col + 1;
    }

    auto operator<=>(Range const &) const = default;
};

struct Book {
    String name;
    Vec<Sheet> sheets = {
        {"Sheet 1"s},
        {"Sheet 2"s},
        {"Sheet 3"s},
    };
};

struct State {
    Book book;
    usize active = 0;
    Opt<Range> selection = NONE;
    bool propertiesVisible = false;

    State() {
        activeSheet().recompute();
    }

    Sheet &activeSheet() {
        return book.sheets[active];
    }

    Sheet const &activeSheet() const {
        return book.sheets[active];
    }
};

// MARK: Actions ---------------------------------------------------------------

struct UpdateSelection {
    Opt<Range> range;
};

// Cells

struct UpdateValue {
    Range range;
    Value value;
};

template <typename TAG, typename T>
struct UpdateStyleField {
    T value;
};

using UpdateStyleFg = UpdateStyleField<struct UpdateStyleFgTag, Gfx::Color>;
using UpdateStyleBg = UpdateStyleField<struct UpdateStyleBgTag, Gfx::Color>;
using UpdateStyleHalign = UpdateStyleField<struct UpdateStyleHalignTag, Align>;
using UpdateStyleValign = UpdateStyleField<struct UpdateStyleValignTag, Align>;
using UpdateStyleFont = UpdateStyleField<struct UpdateStyleFontTag, Opt<Text::Font>>;
using UpdateStyleBorders = UpdateStyleField<struct UpdateStyleBordersTag, Borders>;

// Sheets

struct NewSheet {};

struct DeleteSheet {
    usize index;
};

struct RenameSheet {
    usize index;
    String name;
};

struct SwitchSheet {
    usize index;
};

struct ToggleProperties {};

using Action = Union<
    UpdateSelection,

    UpdateValue,

    UpdateStyleFg,
    UpdateStyleBg,
    UpdateStyleHalign,
    UpdateStyleValign,
    UpdateStyleFont,
    UpdateStyleBorders,

    ToggleProperties,
    SwitchSheet>;

Ui::Task<Action> reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

Res<> save(Model const &model, Io::Writer &writer);

Res<Model> load(Io::Reader &reader);

} // namespace Hideo::Spreadsheet
