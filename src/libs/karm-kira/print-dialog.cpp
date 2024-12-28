#include <karm-app/form-factor.h>
#include <karm-print/paper.h>
#include <karm-ui/layout.h>
#include <karm-ui/popover.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>

#include "checkbox.h"
#include "dialog.h"
#include "print-dialog.h"
#include "row.h"
#include "select.h"

namespace Karm::Kira {

// MARK: Model -----------------------------------------------------------------

struct State {
    PrintPreview preview;
    Print::Settings settings = {};
    Vec<Print::Page> pages = preview(settings);
};

struct ChangePaper {
    Print::PaperStock paper;
};

struct ChangeOrientation {
    Print::Orientation orientation;
};

struct ChangeMargin {
    Print::Margins margins;
};

struct ToggleHeaderFooter {};

struct ToggleBackgroundGraphics {};

struct ChangeScale {
    f64 scale;
};

using Action = Union<
    ChangePaper,
    ChangeOrientation,
    ChangeMargin,
    ToggleHeaderFooter,
    ToggleBackgroundGraphics,
    ChangeScale>;

static Ui::Task<Action> reduce(State &s, Action a) {
    bool shouldUpdatePreview = false;

    if (auto changePaper = a.is<ChangePaper>()) {
        s.settings.paper = changePaper->paper;
        shouldUpdatePreview = true;
    } else if (auto changeOrientation = a.is<ChangeOrientation>()) {
        s.settings.orientation = changeOrientation->orientation;
        shouldUpdatePreview = true;
    } else if (auto changeMargin = a.is<ChangeMargin>()) {
        s.settings.margins = changeMargin->margins;
        shouldUpdatePreview = true;
    } else if (a.is<ToggleHeaderFooter>()) {
        s.settings.headerFooter = not s.settings.headerFooter;
        shouldUpdatePreview = true;
    } else if (a.is<ToggleBackgroundGraphics>()) {
        s.settings.backgroundGraphics = not s.settings.backgroundGraphics;
        shouldUpdatePreview = true;
    } else if (auto changeScale = a.is<ChangeScale>()) {
        s.settings.scale = clamp(changeScale->scale, 0.1, 10.);
        shouldUpdatePreview = true;
    }

    if (shouldUpdatePreview) {
        auto settings = s.settings;
        if (settings.orientation == Print::Orientation::LANDSCAPE)
            settings.paper = s.settings.paper.landscape();
        s.pages = s.preview(settings);
    }

    return NONE;
}

using Model = Ui::Model<State, Action, reduce>;

// MARK: Dialog ----------------------------------------------------------------

Ui::Child _printSelect(State const &s, usize index) {
    return Ui::hflow(
               8,
               Math::Align::CENTER,
               checkbox(true, NONE),
               Ui::labelMedium("Page {} of {}", index + 1, s.pages.len())
           ) |
           Ui::box({
               .margin = 8,
               .padding = Math::Insetsi{6, 12, 6, 6},
               .borderRadii = 9,
               .borderWidth = 1,
               .borderFill = Ui::GRAY800,
               .backgroundFill = Ui::GRAY900,
           });
}

Ui::Child _printPaper(State const &s, usize index) {
    auto scale = 1.;

    auto paper = s.settings.paper;
    if (s.settings.orientation == Print::Orientation::LANDSCAPE)
        paper = paper.landscape();

    auto isMobile = App::useFormFactor() == App::FormFactor::MOBILE;
    if (isMobile)
        scale = 0.5;

    Math::Vec2f previewSize{
        320 * scale,
        320 * (1. / paper.aspect()) * scale,
    };

    return Ui::stack(
               Ui::canvas(
                   s.pages[index].content(),
                   {
                       .showBackgroundGraphics = s.settings.backgroundGraphics,
                   }
               ) | Ui::box({
                       .borderWidth = 1,
                       .borderFill = Ui::GRAY50.withOpacity(0.1),
                       .backgroundFill = Gfx::WHITE,
                   }),
               _printSelect(s, index) | Ui::align(Math::Align::BOTTOM_END)
           ) |
           Ui::pinSize(previewSize.cast<isize>());
}

Ui::Child _printPreviewMobile(State const &s) {
    Ui::Children pages;
    for (usize i = 0; i < s.pages.len(); ++i) {
        pages.pushBack(_printPaper(s, i));
    }

    return Ui::hflow(
               8,
               Math::Align::CENTER,
               std::move(pages)
           ) |
           Ui::insets(32) |
           Ui::hscroll() |
           Ui::box(
               {
                   .backgroundFill = Ui::GRAY950,
               }
           );
}

Ui::Child _printPreview(State const &s) {
    Ui::Children pages;
    for (usize i = 0; i < s.pages.len(); ++i) {
        pages.pushBack(_printPaper(s, i));
    }

    return Ui::vflow(
               8,
               Math::Align::CENTER,
               std::move(pages)
           ) |
           Ui::insets(32) |
           Ui::vscroll() |
           Ui::box(
               {
                   .backgroundFill = Ui::GRAY950,
               }
           );
}

Ui::Child _destinationSelect() {
    return select(
        selectValue("Save as PDF"s),
        [] -> Ui::Children {
            return {
                selectGroup({
                    selectLabel("Save as"s),
                    selectItem(Ui::NOP, "Save as PDF"s),
                    selectItem(Ui::NOP, "Save as Image"s),
                }),
                Ui::separator(),
                selectGroup({
                    selectLabel("Printers"s),
                    selectItem(Ui::NOP, "Epson ET-8550"s),
                    selectItem(Ui::NOP, "HP DeskJet 2700"s),
                }),
                Ui::separator(),
                selectItem(Ui::NOP, "Add printer..."s)

            };
        }
    );
}

Ui::Child _paperSelect(State const &s) {
    return select(selectValue(s.settings.paper.name), [] -> Ui::Children {
        Vec<Ui::Child> groups;

        bool first = false;
        for (auto &serie : Print::SERIES) {
            Vec<Ui::Child> items;
            items.pushBack(selectLabel(serie.name));
            for (auto const &stock : serie.stocks) {
                items.pushBack(selectItem(Model::bind<ChangePaper>(stock), stock.name));
            }

            if (not first)
                groups.pushBack(Ui::separator());
            groups.pushBack(selectGroup(std::move(items)));

            first = false;
        }

        return groups;
    });
}

Ui::Child _printSettings(State const &s) {
    return Ui::vflow(
        rowContent(
            NONE,
            "Destination"s,
            NONE,
            _destinationSelect()
        ),
        selectRow(
            selectValue("1"s),
            [] -> Ui::Children {
                return {
                    selectItem(Ui::NOP, "1"s),
                    selectItem(Ui::NOP, "2"s),
                    selectItem(Ui::NOP, "3"s),
                    selectItem(Ui::NOP, "4"s),
                    selectItem(Ui::NOP, "5"s),
                };
            },
            "Copies"s
        ),
        selectRow(
            selectValue("All"s),
            [] -> Ui::Children {
                return {
                    selectItem(Ui::NOP, "All"s),
                    selectItem(Ui::NOP, "Odd pages only"s),
                    selectItem(Ui::NOP, "Even pages only"s),
                    selectItem(Ui::NOP, "Custom"s),

                };
            },
            "Pages"s
        ),
        selectRow(
            selectValue(
                s.settings.orientation == Print::Orientation::PORTRAIT
                    ? "Portrait"s
                    : "Landscape"s
            ),
            [] -> Ui::Children {
                return {
                    selectItem(Model::bind<ChangeOrientation>(Print::Orientation::PORTRAIT), "Portrait"s),
                    selectItem(Model::bind<ChangeOrientation>(Print::Orientation::LANDSCAPE), "Landscape"s),

                };
            },
            "Orientation"s
        ),
        Ui::separator(),
        treeRow(
            NONE,
            "More settings"s,
            NONE,
            Karm::Ui::Slots{[&] -> Ui::Children {
                return {
                    rowContent(
                        NONE,
                        "Paper"s,
                        NONE,
                        _paperSelect(s)
                    ),
                    selectRow(
                        selectValue("1"s),
                        [] -> Ui::Children {
                            return {
                                selectItem(Ui::NOP, "1"s),
                                selectItem(Ui::NOP, "2"s),
                                selectItem(Ui::NOP, "4"s),
                                selectItem(Ui::NOP, "6"s),
                                selectItem(Ui::NOP, "9"s),
                                selectItem(Ui::NOP, "16"s),
                            };
                        },
                        "Page per sheet"s
                    ),
                    selectRow(
                        selectValue(Io::format("{}", Io::cased(s.settings.margins, Io::Case::CAPITAL)).unwrap()),
                        [] -> Ui::Children {
                            return {
                                selectItem(Model::bind<ChangeMargin>(Print::Margins::NONE), "None"s),
                                selectItem(Model::bind<ChangeMargin>(Print::Margins::MINIMUM), "Minimum"s),
                                selectItem(Model::bind<ChangeMargin>(Print::Margins::DEFAULT), "Default"s),
                                selectItem(Model::bind<ChangeMargin>(Print::Margins::CUSTOM), "Custom"s),
                            };
                        },
                        "Margins"s
                    ),

                    numberRow(
                        s.settings.scale,
                        [](auto &n, f64 scale) {
                            Model::bubble<ChangeScale>(n, ChangeScale{scale});
                        },
                        0.1,
                        "Scale"s
                    ),

                    checkboxRow(
                        s.settings.headerFooter,
                        [&](auto &n, ...) {
                            Model::bubble<ToggleHeaderFooter>(n);
                        },
                        "Header and footers"s
                    ),
                    checkboxRow(
                        s.settings.backgroundGraphics,
                        [&](auto &n, ...) {
                            Model::bubble<ToggleBackgroundGraphics>(n);
                        },
                        "Background graphics"s
                    ),
                };
            }}
        )
    );
}

Ui::Child _printControls(State const &s) {
    return _printSettings(s) |
           Ui::vscroll() |
           Ui::grow() |
           Ui::minSize({320, Ui::UNCONSTRAINED});
}

Ui::Child _printDialog(State const &s) {
    return dialogContent({
        dialogTitleBar("Print"s),
        Ui::hflow(
            _printPreview(s),
            _printControls(s) | Ui::grow()
        ) | Ui::maxSize({Ui::UNCONSTRAINED, 500}) |
            Ui::grow(),
        Ui::separator(),
        dialogFooter({
            dialogCancel(),
            dialogAction(Ui::NOP, "Print"s),
        }),
    });
}

Ui::Child _printDialogMobile(State const &s) {
    return dialogContent({
        dialogTitleBar("Print"s),
        Ui::separator(),
        Ui::vflow(
            _printPreviewMobile(s),
            Ui::separator(),
            titleRow("Settings"s),
            _printSettings(s)
        ) | Ui::minSize(500) |
            Ui::vscroll() |
            Ui::grow(),
        Ui::separator(),
        dialogFooter({
            dialogCancel(),
            dialogAction(Ui::NOP, "Print"s),
        }),
    });
}

Ui::Child printDialog(PrintPreview preview) {
    return Ui::reducer<Model>({preview}, [](State const &s) {
        auto isMobile = App::useFormFactor() == App::FormFactor::MOBILE;
        if (isMobile)
            return _printDialogMobile(s);
        return _printDialog(s) | Ui::popoverLayer();
    });
}

} // namespace Karm::Kira
