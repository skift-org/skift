#include <karm-app/form-factor.h>
#include <karm-print/paper.h>
#include <karm-ui/layout.h>
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
    Vec<Strong<Scene::Page>> pages = preview(settings);
};

using Action = Union<None>;

void reduce(State &, Action) {
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
    auto isMobile = App::useFormFactor() == App::FormFactor::MOBILE;
    if (isMobile) {
        scale = 0.5;
    }

    return Ui::stack(
               Ui::canvas(s.pages[index]) |
                   Ui::box({
                       .borderRadii = 6,
                       .borderWidth = 1,
                       .borderFill = Ui::GRAY50.withOpacity(0.1),
                       .backgroundFill = Gfx::WHITE,
                   }),
               _printSelect(s, index) | Ui::align(Math::Align::BOTTOM_END)
           ) |
           Ui::pinSize(Math::Vec2f{320 * scale, 452 * scale}.cast<isize>());
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

Ui::Child _paperSelect() {
    return select(selectValue("A4"s), [] -> Ui::Children {
        Vec<Ui::Child> groups;

        for (auto &serie : Print::SERIES) {
            Vec<Ui::Child> items;
            items.pushBack(selectLabel(serie.name));
            for (auto const &stock : serie.stocks) {
                items.pushBack(selectItem(Ui::NOP, stock.name));
            }

            groups.pushBack(selectGroup(std::move(items)));
        }

        return groups;
    });
}

Ui::Child _printSettings() {
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
            selectValue("Portrait"s),
            [] -> Ui::Children {
                return {
                    selectItem(Ui::NOP, "Portrait"s),
                    selectItem(Ui::NOP, "Landscape"s),

                };
            },
            "Orientation"s
        ),
        Ui::separator(),
        treeRow(
            NONE,
            "More settings"s,
            NONE,
            Karm::Ui::Slots{[] -> Ui::Children {
                return {
                    rowContent(
                        NONE,
                        "Paper"s,
                        NONE,
                        _paperSelect()
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
                        selectValue("Default"s),
                        [] -> Ui::Children {
                            return {
                                selectItem(Ui::NOP, "None"s),
                                selectItem(Ui::NOP, "Small"s),
                                selectItem(Ui::NOP, "Default"s),
                                selectItem(Ui::NOP, "Large"s),
                            };
                        },
                        "Margins"s
                    ),

                    checkboxRow(true, NONE, "Header and footers"s),
                    checkboxRow(false, NONE, "Background graphics"s),
                };
            }}
        )
    );
}

Ui::Child _printControls() {
    return _printSettings() |
           Ui::vscroll() |
           Ui::grow() |
           Ui::minSize({320, Ui::UNCONSTRAINED});
}

Ui::Child _printDialog(State const &s) {
    return dialogContent({
        dialogTitleBar("Print"s),
        Ui::hflow(
            _printPreview(s),
            _printControls() | Ui::grow()
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
            _printSettings()
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
        return _printDialog(s);
    });
}

} // namespace Karm::Kira
