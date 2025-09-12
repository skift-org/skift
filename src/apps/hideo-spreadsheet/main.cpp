#include <karm-sys/entry.h>

#include "app.h"

import Mdi;
import Karm.Ui;
import Karm.Kira;

namespace Hideo::Spreadsheet {

Ui::Child formula() {
    return Ui::box(
               {
                   .borderRadii = 4,
                   .borderWidth = 1,
                   .backgroundFill = Ui::GRAY800,
               },
               Ui::hflow(
                   Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FUNCTION),
                   Ui::grow(NONE)
               )
           ) |
           Ui::focusable();
}

Ui::Child toolbar(State const& state) {
    return Kr::toolbar({
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FILE),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FOLDER),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::CONTENT_SAVE_PLUS),
        Kr::separator(),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::UNDO),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::REDO),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FORMAT_PAINT),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FORMAT_CLEAR),
        Kr::separator(),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FILTER),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::SIGMA),
        Kr::separator(),
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

            Kr::colorRow(Gfx::RED, Ui::SINK<Gfx::Color>, "Color"s),

            Kr::rowContent(
                NONE,
                "Format"s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FORMAT_BOLD),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FORMAT_ITALIC),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FORMAT_UNDERLINE),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FORMAT_STRIKETHROUGH)
                )
            ),

            Kr::rowContent(
                NONE,
                "Wrapping"s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::WRAP),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::WRAP_DISABLED)
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

            Kr::separator(),

            Kr::titleRow("Cell Properties"s),
            Kr::colorRow(Gfx::RED, Ui::SINK<Gfx::Color>, "Background Color"s),
            Kr::colorRow(Gfx::RED, Ui::SINK<Gfx::Color>, "Border Color"s),

            Kr::rowContent(
                NONE,
                "Borders"s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::BORDER_ALL),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::BORDER_NONE),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::BORDER_VERTICAL),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::BORDER_HORIZONTAL)
                )
            ),

            Kr::rowContent(
                NONE,
                ""s,
                NONE,
                Ui::hflow(
                    4,
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::BORDER_TOP),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::BORDER_BOTTOM),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::BORDER_LEFT),
                    Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::BORDER_RIGHT)
                )
            ),

            Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::TABLE_MERGE_CELLS, "Merge Cells"),

            Kr::separator(),

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
            Ui::SINK<>,
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
            body = hflow(body, Kr::separator(), properties());
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
