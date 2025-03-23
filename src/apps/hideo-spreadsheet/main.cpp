#include <karm-kira/row.h>
#include <karm-kira/scaffold.h>
#include <karm-kira/titlebar.h>
#include <karm-kira/toolbar.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/focus.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>
#include <mdi/align-horizontal-center.h>
#include <mdi/align-horizontal-left.h>
#include <mdi/align-horizontal-right.h>
#include <mdi/align-vertical-bottom.h>
#include <mdi/align-vertical-center.h>
#include <mdi/align-vertical-top.h>
#include <mdi/border-all.h>
#include <mdi/border-bottom.h>
#include <mdi/border-horizontal.h>
#include <mdi/border-left.h>
#include <mdi/border-none.h>
#include <mdi/border-right.h>
#include <mdi/border-top.h>
#include <mdi/border-vertical.h>
#include <mdi/brush-variant.h>
#include <mdi/content-save-plus.h>
#include <mdi/content-save.h>
#include <mdi/file.h>
#include <mdi/filter.h>
#include <mdi/folder.h>
#include <mdi/format-bold.h>
#include <mdi/format-clear.h>
#include <mdi/format-italic.h>
#include <mdi/format-paint.h>
#include <mdi/format-strikethrough.h>
#include <mdi/format-underline.h>
#include <mdi/function.h>
#include <mdi/plus.h>
#include <mdi/redo.h>
#include <mdi/sigma.h>
#include <mdi/table-merge-cells.h>
#include <mdi/table.h>
#include <mdi/undo.h>
#include <mdi/wrap-disabled.h>
#include <mdi/wrap.h>

#include "app.h"

namespace Hideo::Spreadsheet {

Ui::Child formula() {
    return Ui::box(
               {
                   .borderRadii = 4,
                   .borderWidth = 1,
                   .backgroundFill = Ui::GRAY800,
               },
               Ui::hflow(
                   Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FUNCTION),
                   Ui::grow(NONE)
               )
           ) |
           Ui::focusable();
}

Ui::Child toolbar(State const& state) {
    return Kr::toolbar({
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
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FILTER),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::SIGMA),
        Ui::separator(),
        Ui::empty(4),
        formula() | Ui::grow(),
        Ui::empty(4),
        Ui::button(
            Model::bind<ToggleProperties>(),
            state.propertiesVisible
                ? Ui::ButtonStyle::regular()
                : Ui::ButtonStyle::subtle(),
            Mdi::BRUSH_VARIANT
        ),
    });
}

Ui::Child properties() {
    return Ui::vscroll(
        Ui::vflow(
            Kr::titleRow("Text Properties"s),

            Kr::colorRow(Gfx::RED, Ui::IGNORE<Gfx::Color>, "Color"s),

            Kr::rowContent(
                NONE,
                "Format"s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_BOLD),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_ITALIC),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_UNDERLINE),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FORMAT_STRIKETHROUGH)
                )
            ),

            Kr::rowContent(
                NONE,
                "Wrapping"s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::WRAP),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::WRAP_DISABLED)
                )
            ),

            Kr::rowContent(
                NONE,
                "Horizontal Align"s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Model::bind<UpdateStyleHalign>(Align::START), Ui::ButtonStyle::subtle(), Mdi::ALIGN_HORIZONTAL_LEFT),
                    Ui::button(Model::bind<UpdateStyleHalign>(Align::CENTER), Ui::ButtonStyle::subtle(), Mdi::ALIGN_HORIZONTAL_CENTER),
                    Ui::button(Model::bind<UpdateStyleHalign>(Align::END), Ui::ButtonStyle::subtle(), Mdi::ALIGN_HORIZONTAL_RIGHT)
                )
            ),

            Kr::rowContent(
                NONE,
                "Vertical Align"s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Model::bind<UpdateStyleValign>(Align::START), Ui::ButtonStyle::subtle(), Mdi::ALIGN_VERTICAL_TOP),
                    Ui::button(Model::bind<UpdateStyleValign>(Align::CENTER), Ui::ButtonStyle::subtle(), Mdi::ALIGN_VERTICAL_CENTER),
                    Ui::button(Model::bind<UpdateStyleValign>(Align::END), Ui::ButtonStyle::subtle(), Mdi::ALIGN_VERTICAL_BOTTOM)
                )
            ),

            Ui::separator(),

            Kr::titleRow("Cell Properties"s),
            Kr::colorRow(Gfx::RED, Ui::IGNORE<Gfx::Color>, "Background Color"s),
            Kr::colorRow(Gfx::RED, Ui::IGNORE<Gfx::Color>, "Border Color"s),

            Kr::rowContent(
                NONE,
                "Borders"s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_ALL),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_NONE),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_VERTICAL),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_HORIZONTAL)
                )
            ),

            Kr::rowContent(
                NONE,
                ""s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_TOP),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_BOTTOM),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_LEFT),
                    Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::BORDER_RIGHT)
                )
            ),

            Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::TABLE_MERGE_CELLS, "Merge Cells"),

            Ui::separator(),

            Kr::titleRow("Sheet Properties"s)
        )
    );
}

Ui::Child tabItem(State const& state, Sheet const& sheet, usize index) {
    return Ui::button(
        Model::bind<SwitchSheet>(index),
        state.active == index
            ? Ui::ButtonStyle::secondary()
            : Ui::ButtonStyle::subtle(),
        sheet.name
    );
}

Ui::Child tabs(State const& state) {
    return Ui::hflow(
        4,
        Ui::hflow(
            iter(state.book.sheets)
                .mapi([&](auto const& s, usize i) {
                    return tabItem(state, s, i);
                })
                .collect<Ui::Children>()
        ),

        Ui::button(
            Ui::NOP,
            Ui::ButtonStyle::subtle(),
            Mdi::PLUS
        )
    );
}

Ui::Child app() {
    return Ui::reducer<Model>({}, [](auto const& s) {
        auto tb = Kr::titlebar(Mdi::TABLE, "Spreadsheet"s, tabs(s));
        auto body = table(s) | Ui::grow();
        if (s.propertiesVisible) {
            body = hflow(body, Ui::separator(), properties());
        }

        return Ui::vflow(
                   tb,
                   toolbar(s),
                   body | Ui::grow()
               ) |
               Ui::pinSize({800, 600}) | Ui::dialogLayer();
    });
}

} // namespace Hideo::Spreadsheet

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    co_return co_await Ui::runAsync(ctx, Hideo::Spreadsheet::app());
}
