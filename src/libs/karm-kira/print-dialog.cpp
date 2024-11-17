#include <karm-app/form-factor.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>

#include "dialog.h"
#include "print-dialog.h"
#include "row.h"
#include "select.h"

namespace Karm::Kira {

Ui::Child _printPaper() {
    return Ui::empty({320, 452}) |
           Ui::box({
               .borderRadii = 6,
               .backgroundFill = Ui::GRAY50,
           });
}

Ui::Child _printPreview() {
    return Ui::vflow(
               8,
               Math::Align::CENTER,
               _printPaper(),
               _printPaper(),
               _printPaper()
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

Ui::Child _printSettings() {
    return Ui::vflow(
        selectRow(
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
            },
            "Destination"s
        ),
        selectRow(
            selectValue("All"s),
            [] -> Ui::Children {
                return {
                    selectItem(Ui::NOP, "All"s),
                    selectItem(Ui::NOP, "Odd pages only"s),
                    selectItem(Ui::NOP, "Even pages only"s),

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

Ui::Child printDialog() {
    auto isMobile = App::useFormFactor() == App::FormFactor::MOBILE;

    return dialogContent({
        dialogTitleBar("Print"s),
        Ui::hflow(
            isMobile ? Ui::empty() : _printPreview(),
            _printControls()
        ) | Ui::maxSize({Ui::UNCONSTRAINED, 500}) |
            Ui::grow(),
        Ui::separator(),
        dialogFooter({
            dialogCancel(),
            dialogAction(Ui::NOP, "Print"s),
        }),
    });
}

} // namespace Karm::Kira
