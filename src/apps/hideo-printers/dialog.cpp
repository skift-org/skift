module;

#include <karm-gfx/colors.h>
#include <karm-math/align.h>
#include <karm-sys/file.h>
#include <karm-sys/proc.h>

export module Hideo.Printers:dialog;

import Karm.Core;
import Karm.App;
import Karm.Print;
import Karm.Kira;
import Karm.Ui;

using namespace Karm;

namespace Hideo::Printers {

// MARK: Model -----------------------------------------------------------------

enum struct PrintAction {
    PRINT,
    STOP
};

export using PrintPreview = SharedFunc<Vec<Print::Page>(Print::Settings const&)>;

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
    ChangeScale,
    PrintAction>;

void _printPDF(State const& s);

static Ui::Task<Action> reduce(State& s, Action a) {
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
    } else if (a.is<PrintAction>()) {
        _printPDF(s);
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

Ui::Child _printSelect(State const& s, usize index) {
    return Ui::hflow(
               8,
               Math::Align::CENTER,
               Kr::checkbox(true, Ui::SINK<bool>),
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

Ui::Child _printPaper(State const& s, usize index) {
    auto scale = 1.;

    auto paper = s.settings.paper;
    if (s.settings.orientation == Print::Orientation::LANDSCAPE)
        paper = paper.landscape();

    auto isMobile = App::formFactor == App::FormFactor::MOBILE;
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

Ui::Child _printPreviewMobile(State const& s) {
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

Ui::Child _printPreview(State const& s) {
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

void _printPDF(State const& s) {
    auto printer = Print::FilePrinter::create(Ref::Uti::PUBLIC_PDF).unwrap();
    for (usize i = 0; i < s.pages.len(); ++i) {
        auto page = s.pages[i];
        page.print(
            *printer,
            {
                s.settings.backgroundGraphics,
            }
        );
    }
    printer->save(Ref::parseUrlOrPath("./output.pdf", Sys::pwd().unwrap())).unwrap();
}

Ui::Child _destinationSelect() {
    return Kr::select(
        Kr::selectValue("Save as PDF"s),
        [] -> Ui::Children {
            return {
                Kr::selectGroup({
                    Kr::selectLabel("Save as"s),
                    Kr::selectItem(Ui::SINK<>, "Save as PDF"s),
                    Kr::selectItem(Ui::SINK<>, "Save as Image"s),
                }),
                Kr::separator(),
                Kr::selectGroup({
                    Kr::selectLabel("Printers"s),
                    Kr::selectItem(Ui::SINK<>, "Epson ET-8550"s),
                    Kr::selectItem(Ui::SINK<>, "HP DeskJet 2700"s),
                }),
                Kr::separator(),
                Kr::selectItem(Ui::SINK<>, "Add printer..."s)
            };
        }
    );
}

Ui::Child _paperSelect(State const& s) {
    return Kr::select(Kr::selectValue(s.settings.paper.name), [] -> Ui::Children {
        Vec<Ui::Child> groups;

        bool first = false;
        for (auto& serie : Print::SERIES) {
            Vec<Ui::Child> items;
            items.pushBack(Kr::selectLabel(serie.name));
            for (auto const& stock : serie.stocks) {
                items.pushBack(Kr::selectItem(Model::bind<ChangePaper>(stock), stock.name));
            }

            if (not first)
                groups.pushBack(Kr::separator());
            groups.pushBack(Kr::selectGroup(std::move(items)));

            first = false;
        }

        return groups;
    });
}

Ui::Child _printSettings(State const& s) {
    return Ui::vflow(
        Kr::rowContent(
            NONE,
            "Destination"s,
            NONE,
            _destinationSelect()
        ),
        Kr::selectRow(
            Kr::selectValue("1"s),
            [] -> Ui::Children {
                return {
                    Kr::selectItem(Ui::SINK<>, "1"s),
                    Kr::selectItem(Ui::SINK<>, "2"s),
                    Kr::selectItem(Ui::SINK<>, "3"s),
                    Kr::selectItem(Ui::SINK<>, "4"s),
                    Kr::selectItem(Ui::SINK<>, "5"s),
                };
            },
            "Copies"s
        ),
        Kr::selectRow(
            Kr::selectValue("All"s),
            [] -> Ui::Children {
                return {
                    Kr::selectItem(Ui::SINK<>, "All"s),
                    Kr::selectItem(Ui::SINK<>, "Odd pages only"s),
                    Kr::selectItem(Ui::SINK<>, "Even pages only"s),
                    Kr::selectItem(Ui::SINK<>, "Custom"s),
                };
            },
            "Pages"s
        ),
        Kr::selectRow(
            Kr::selectValue(
                s.settings.orientation == Print::Orientation::PORTRAIT
                    ? "Portrait"s
                    : "Landscape"s
            ),
            [] -> Ui::Children {
                return {
                    Kr::selectItem(Model::bind<ChangeOrientation>(Print::Orientation::PORTRAIT), "Portrait"s),
                    Kr::selectItem(Model::bind<ChangeOrientation>(Print::Orientation::LANDSCAPE), "Landscape"s),

                };
            },
            "Orientation"s
        ),
        Kr::separator(),
        Kr::treeRow(
            NONE,
            "More settings"s,
            NONE,
            Ui::Slots{[&] -> Ui::Children {
                return {
                    Kr::rowContent(
                        NONE,
                        "Paper"s,
                        NONE,
                        _paperSelect(s)
                    ),
                    Kr::selectRow(
                        Kr::selectValue("1"s),
                        [] -> Ui::Children {
                            return {
                                Kr::selectItem(Ui::SINK<>, "1"s),
                                Kr::selectItem(Ui::SINK<>, "2"s),
                                Kr::selectItem(Ui::SINK<>, "4"s),
                                Kr::selectItem(Ui::SINK<>, "6"s),
                                Kr::selectItem(Ui::SINK<>, "9"s),
                                Kr::selectItem(Ui::SINK<>, "16"s),
                            };
                        },
                        "Page per sheet"s
                    ),
                    Kr::selectRow(
                        Kr::selectValue(Io::format("{}", Io::cased(s.settings.margins, Io::Case::CAPITAL))),
                        [] -> Ui::Children {
                            return {
                                Kr::selectItem(Model::bind<ChangeMargin>(Print::Margins::NONE), "None"s),
                                Kr::selectItem(Model::bind<ChangeMargin>(Print::Margins::MINIMUM), "Minimum"s),
                                Kr::selectItem(Model::bind<ChangeMargin>(Print::Margins::DEFAULT), "Default"s),
                                Kr::selectItem(Model::bind<ChangeMargin>(Print::Margins::CUSTOM), "Custom"s),
                            };
                        },
                        "Margins"s
                    ),

                    Kr::numberRow(
                        s.settings.scale,
                        [](auto& n, f64 scale) {
                            Model::bubble<ChangeScale>(n, ChangeScale{scale});
                        },
                        0.1,
                        "Scale"s
                    ),

                    Kr::checkboxRow(
                        s.settings.headerFooter,
                        [&](auto& n, ...) {
                            Model::bubble<ToggleHeaderFooter>(n);
                        },
                        "Header and footers"s
                    ),
                    Kr::checkboxRow(
                        s.settings.backgroundGraphics,
                        [&](auto& n, ...) {
                            Model::bubble<ToggleBackgroundGraphics>(n);
                        },
                        "Background graphics"s
                    ),
                };
            }}
        )
    );
}

Ui::Child _printControls(State const& s) {
    return _printSettings(s) |
           Ui::vscroll() |
           Ui::grow() |
           Ui::minSize({320, Ui::UNCONSTRAINED});
}

Ui::Child _printDialog(State const& s) {
    return Kr::dialogContent({
        Kr::dialogTitleBar("Print"s),
        Ui::hflow(
            _printPreview(s),
            _printControls(s) | Ui::grow()
        ) | Ui::maxSize({Ui::UNCONSTRAINED, 500}) |
            Ui::grow(),
        Kr::separator(),
        Kr::dialogFooter({
            Kr::dialogCancel(),
            Kr::dialogAction(Model::bind(PrintAction::PRINT), "Print"s),
        }),
    });
}

Ui::Child _printDialogMobile(State const& s) {
    return Kr::dialogContent({
        Kr::dialogTitleBar("Print"s),
        Kr::separator(),
        Ui::vflow(
            _printPreviewMobile(s),
            Kr::separator(),
            Kr::titleRow("Settings"s),
            _printSettings(s)
        ) | Ui::minSize(500) |
            Ui::vscroll() |
            Ui::grow(),
        Kr::separator(),
        Kr::dialogFooter({
            Kr::dialogCancel(),
            Kr::dialogAction(Model::bind(PrintAction::PRINT), "Print"s),
        }),
    });
}

export Ui::Child printDialog(PrintPreview preview) {
    return Ui::reducer<Model>({preview}, [](State const& s) {
        auto isMobile = App::formFactor == App::FormFactor::MOBILE;
        if (isMobile)
            return _printDialogMobile(s);
        return _printDialog(s) | Ui::popoverLayer();
    });
}

} // namespace Hideo::Printers
