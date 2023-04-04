#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/scafold.h>
#include <karm-ui/view.h>

#include "app.h"

namespace Spreadsheet {

struct SheetView : public Ui::View<SheetView> {
    Sheet _sheet;
    Range _sel = {{3, 3}, {6, 5}};
    Math::Vec2i _scroll;
    Ui::MouseListener _mouseListener;

    SheetView(Sheet const &sheet)
        : _sheet(sheet) {}

    void reconcile(SheetView &o) override {
        _sheet = o._sheet;
    }

    /* --- Geometry --- */

    Math::Recti colHeaderBound(usize col) {
        return {
            _sheet.cols[col].x,
            0,
            _sheet.cols[col].width,
            CELL_HEIGHT,
        };
    }

    Math::Recti rowHeaderBound(usize row) {
        return {
            0,
            _sheet.rows[row].y,
            CELL_WIDTH,
            _sheet.rows[row].height,
        };
    }

    Math::Recti cellBound(usize row, usize col) {
        return {
            _sheet.cols[col].x + CELL_WIDTH,
            _sheet.rows[row].y + CELL_HEIGHT,
            _sheet.cols[col].width,
            _sheet.rows[row].height,
        };
    }

    /* --- Events --- */

    void event(Events::Event &e) override {
        e.handle<Events::MouseEvent>([&](Events::MouseEvent const &m) {
            auto pos = m.pos - bound().topStart();
            if (bound().contains(m.pos)) {
                if (m.type == Events::MouseEvent::PRESS) {
                    auto cell = _sheet.cellAt(pos - Math::Vec2i{CELL_WIDTH, CELL_HEIGHT});
                    debug("press");
                    if (cell) {
                        _sel = *cell;
                        Ui::shouldRepaint(*this);
                    }
                } else if (m.type == Events::MouseEvent::MOVE && ((m.buttons & Events::Button::LEFT) == Events::Button::LEFT)) {
                    auto cell = _sheet.cellAt(pos - Math::Vec2i{CELL_WIDTH, CELL_HEIGHT});
                    debug("move");
                    if (cell) {
                        _sel.end = *cell;
                        Ui::shouldRepaint(*this);
                    }
                }

                return true;
            }

            return false;
        });
    }

    /* --- Painting --- */

    void paintCell(Gfx::Context &g, Cell const &, Math::Recti bound) {
        g.rect(bound.cast<f64>());
    }

    void paintColHeader(Gfx::Context &g, usize idx) {
        auto col = _sheet.cols[idx];
        auto bound = colHeaderBound(idx);
        auto sep = Math::Edgei{
            col.x + col.width - 1,
            0,
            col.x + col.width - 1,
            _bound.height,
        };

        g.fillStyle(Gfx::ZINC800);
        g.fill(bound);
        g.debugLine(sep, Gfx::WHITE.withOpacity(0.05));
    }

    void paintRowHeader(Gfx::Context &g, usize idx) {
        auto row = _sheet.rows[idx];
        Math::Recti bound = {0, row.y, CELL_WIDTH, row.height};
        Math::Edgei sep = {
            0,
            row.y + row.height - 1,
            _bound.width,
            row.y + row.height - 1,
        };

        g.fillStyle(Gfx::ZINC800);
        g.fill(bound);
        g.debugLine(sep, Gfx::WHITE.withOpacity(0.05));
    }

    void paintSelection(Gfx::Context &g, Range r) {
        auto start = cellBound(r.start.row, r.start.col);
        auto end = cellBound(r.end.row, r.end.col);
        auto all = start.mergeWith(end);

        g.fillStyle(Gfx::BLUE600.withOpacity(0.2));
        g.begin();
        g.rect(all.cast<f64>().grow(3), 4);
        g.rect(start.cast<f64>().grow(3), 4);
        g.fill(Gfx::FillRule::EVENODD);

        g.strokeStyle(Gfx::stroke(Gfx::BLUE500).withAlign(Gfx::OUTSIDE_ALIGN).withWidth(2));
        g.stroke(all.grow(3), 4);

        auto handle = all.bottomEnd() + 3;
        g.fillStyle(Gfx::WHITE);
        g.fill(Math::Ellipsei{handle, 4});

        g.strokeStyle(Gfx::stroke(Gfx::ZINC900).withAlign(Gfx::OUTSIDE_ALIGN).withWidth(1));
        g.stroke();
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        _sheet.recompute(); // TODO: Only recompute when needed.

        g.save();
        g.origin(bound().xy);

        // Draw columns.
        isize headerX = CELL_WIDTH;
        usize index = 0;
        while (headerX < _bound.width &&
               index < _sheet.cols.len()) {

            auto col = _sheet.cols[index];
            Math::Recti colBound = {headerX, 0, col.width, CELL_HEIGHT};

            g.fillStyle(Gfx::ZINC800);
            g.fill(colBound);

            g.debugLine(Math::Edgei{headerX + col.width - 1, 0, headerX + col.width - 1, _bound.height}, Gfx::WHITE.withOpacity(0.05));

            headerX += col.width;
            index++;
        }

        // Draw rows.
        isize headerY = CELL_HEIGHT;
        index = 0;
        while (headerY < _bound.height &&
               index < _sheet.rows.len()) {

            auto row = _sheet.rows[index];
            Math::Recti rowBound = {0, headerY, CELL_WIDTH, row.height};

            g.fillStyle(Gfx::ZINC800);
            g.fill(rowBound);

            g.debugLine(Math::Edgei{0, headerY + row.height - 1, _bound.width, headerY + row.height - 1}, Gfx::WHITE.withOpacity(0.05));

            headerY += row.height;
            index++;
        }

        paintSelection(g, _sel);

        g.restore();
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return {100, 100};
    }
};

Ui::Child toolbar() {
    return Ui::toolbar(
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FILE),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FOLDER),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE_PLUS),
        Ui::separator(),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::UNDO),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::REDO),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_PAINT),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_CLEAR),
        Ui::separator(),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_BOLD),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_ITALIC),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_UNDERLINE),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_STRIKETHROUGH),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_COLOR_TEXT),
        Ui::separator(),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_COLOR_FILL),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_ALL),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::TABLE_MERGE_CELLS),
        Ui::separator(),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ALIGN_HORIZONTAL_LEFT),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ALIGN_VERTICAL_CENTER),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::WRAP),
        Ui::separator(),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::TABLE_HEADERS_EYE),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FILTER),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::SIGMA));
}

Ui::Child sheet(Sheet const &sheet) {
    return makeStrong<SheetView>(sheet);
}

Ui::Child tab(bool selected, String title) {
    auto indicator = Ui::box(Ui::BoxStyle{
                                 .borderRadius = 99,
                                 .backgroundPaint = selected ? Gfx::BLUE600 : Gfx::ALPHA,
                             },
                             Ui::empty(4));

    return Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Ui::vflow(Ui::labelMedium(title) | Ui::center() | Ui::spacing({16, 0}) | Ui::grow(), indicator) | Ui::spacing({6, 6}));
}

Ui::Child bottombar() {
    return Ui::bottombar(
        Ui::button(Ui::NOP, Ui::ButtonStyle::regular(), "Sheet 1"),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), "Sheet 2"),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), "Sheet 3"),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::PLUS));
}

Ui::Child app() {
    return Ui::reducer<Model>({}, [](auto const &s) {
        return Ui::vflow(
                   Ui::titlebar(Mdi::TABLE, "Spreadsheet"),
                   toolbar(),
                   sheet(s.activeSheet()) | Ui::grow(),
                   bottombar()) |
               Ui::minSize({800, 600}) | Ui::dialogLayer();
    });
}

} // namespace Spreadsheet

Res<> entryPoint(CliArgs args) {
    return Ui::runApp(args, Spreadsheet::app());
}
