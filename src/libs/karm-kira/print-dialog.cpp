#include <karm-app/form-factor.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>

#include "checkbox.h"
#include "dialog.h"
#include "print-dialog.h"
#include "row.h"
#include "select.h"

namespace Karm::Kira {

Ui::Child _printSelect(usize index) {
    return Ui::hflow(
               8,
               Math::Align::CENTER,
               checkbox(true, NONE),
               Ui::labelMedium("Page {} of 3", index + 1)
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

Ui::Child _printPaper(usize index) {
    auto scale = 1.;
    auto isMobile = App::useFormFactor() == App::FormFactor::MOBILE;
    if (isMobile) {
        scale = 0.5;
    }

    return Ui::stack(
        Ui::empty(Math::Vec2f{320 * scale, 452 * scale}.cast<isize>()) |
            Ui::box({
                .borderRadii = 6,
                .backgroundFill = Ui::GRAY50,
            }),
        _printSelect(index) | Ui::align(Math::Align::BOTTOM_END)
    );
}

Ui::Child _printPreviewMobile() {
    return Ui::hflow(
               8,
               Math::Align::CENTER,
               _printPaper(0),
               _printPaper(1),
               _printPaper(2)
           ) |
           Ui::insets(32) |
           Ui::hscroll() |
           Ui::box(
               {
                   .borderRadii = {0, 0, 0, 12},
                   .backgroundFill = Ui::GRAY950,
               }
           );
}

Ui::Child _printPreview() {
    return Ui::vflow(
               8,
               Math::Align::CENTER,
               _printPaper(0),
               _printPaper(1),
               _printPaper(2)
           ) |
           Ui::insets(32) |
           Ui::vscroll() |
           Ui::box(
               {
                   .borderRadii = {0, 0, 0, 12},
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
                    selectRow(
                        selectValue("A4"s),
                        [] -> Ui::Children {
                            return {
                                selectItem(Ui::NOP, "A3"s),
                                selectItem(Ui::NOP, "A4"s),
                                selectItem(Ui::NOP, "A5"s),
                                selectItem(Ui::NOP, "Letter"s),
                                selectItem(Ui::NOP, "Legal"s),
                            };
                        },
                        "Paper size"s
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

Ui::Child _printDialog() {
    return dialogContent({
        dialogTitleBar("Print"s),
        Ui::hflow(
            _printPreview(),
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

Ui::Child _printDialogMobile() {
    return dialogContent({
        dialogTitleBar("Print"s),
        Ui::separator(),
        Ui::vflow(
            _printPreviewMobile(),
            Ui::separator(),
            titleRow("Settings"s),
            _printSettings()
        ) | Ui::vscroll() |
            Ui::grow(),
        Ui::separator(),
        dialogFooter({
            dialogCancel(),
            dialogAction(Ui::NOP, "Print"s),
        }),
    });
}

Ui::Child printDialog() {
    auto isMobile = App::useFormFactor() == App::FormFactor::MOBILE;

    if (isMobile) {
        return _printDialogMobile();
    }
    return _printDialog();
}

} // namespace Karm::Kira
