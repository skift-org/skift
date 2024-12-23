#include <karm-ui/anim.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <mdi/chevron-down.h>
#include <mdi/chevron-up.h>
#include <mdi/drag-vertical-variant.h>

#include "checkbox.h"
#include "color-input.h"
#include "number.h"
#include "radio.h"
#include "row.h"
#include "select.h"
#include "slider.h"
#include "toggle.h"

namespace Karm::Kira {

Ui::Child rowContent(Ui::Child child) {
    return child |
           Ui::vcenter() |
           Ui::insets(16) |
           Ui::minSize({Ui::UNCONSTRAINED, 64});
}

Ui::Child rowContent(Opt<Ui::Child> leading, String title, Opt<String> subtitle, Opt<Ui::Child> trailing) {
    auto lead = leading
                    ? *leading |
                          Ui::center() |
                          Ui::sizing(26, {Ui::UNCONSTRAINED, 26}) |
                          Ui::insets({0, 12, 0, 0})
                    : Ui::empty();

    auto t = subtitle
                 ? Ui::vflow(
                       8,
                       Ui::labelMedium(title),
                       Ui::labelSmall(*subtitle)
                   )
                 : Ui::labelMedium(title);

    auto trail = trailing
                     ? *trailing |
                           Ui::center() |
                           Ui::sizing(26, {Ui::UNCONSTRAINED, 26})
                     : Ui::empty();

    return minSize(
        {Ui::UNCONSTRAINED, 48},
        insets(
            {0, 16},
            hflow(
                0,
                Math::Align::VCENTER | Math::Align::HFILL,
                lead,
                t | Ui::grow(),
                trail
            )
        )
    );
}

Ui::Child titleRow(String t) {
    return Ui::titleMedium(t) |
           Ui::insets({16, 12, 8, 12});
}

Ui::Child pressableRow(Ui::OnPress onPress, Opt<Ui::Child> leading, String title, Opt<String> subtitle, Opt<Ui::Child> trailing) {
    return button(
        std::move(onPress),
        Ui::ButtonStyle::subtle(),
        rowContent(
            leading,
            title,
            subtitle,
            trailing
        )
    );
}

Ui::Child buttonRow(Ui::OnPress onPress, Mdi::Icon i, String title, String subtitle) {
    return button(
        std::move(onPress),
        Ui::ButtonStyle::subtle(),
        rowContent(
            Ui::icon(i, 24),
            title,
            subtitle,
            NONE
        )
    );
}

Ui::Child buttonRow(Ui::OnPress onPress, String title, String text) {
    return rowContent(
        NONE,
        title,
        NONE,
        button(std::move(onPress), Ui::ButtonStyle::primary(), text)
    );
}

Ui::Child toggleRow(bool value, Ui::OnChange<bool> onChange, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        Kr::toggle(value, std::move(onChange))
    );
}

Ui::Child checkboxRow(bool value, Ui::OnChange<bool> onChange, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        Kr::checkbox(value, std::move(onChange))
    );
}

Ui::Child radioRow(bool value, Ui::OnChange<bool> onChange, String title) {
    return rowContent(
        Kr::radio(value, std::move(onChange)),
        title,
        NONE,
        NONE
    );
}

Ui::Child sliderRow(f64 value, Ui::OnChange<f64> onChange, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        Kr::slider(
            value,
            std::move(onChange),
            Mdi::DRAG_VERTICAL_VARIANT,
            ""
        ) | Ui::minSize({128, Ui::UNCONSTRAINED})
    );
}

Ui::Child selectRow(Ui::Child value, Ui::Slots options, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        Kr::select(std::move(value), std::move(options))
    );
}

Ui::Child colorRow(Gfx::Color c, Ui::OnChange<Gfx::Color> onChange, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        Kr::colorInput(c, std::move(onChange))
    );
}

Ui::Child numberRow(f64 value, Ui::OnChange<f64> onChange, f64 step, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        number(value, std::move(onChange), step)
    );
}

Ui::Child treeRow(Opt<Ui::Slot> leading, String title, Opt<String> subtitle, Ui::Slot child) {
    return Ui::state(false, [=, leading = std::move(leading), child = std::move(child)](bool state, auto bind) {
        return vflow(
            0,
            pressableRow(
                bind(not state),
                leading(),
                title,
                subtitle,
                Ui::icon(
                    state ? Mdi::CHEVRON_UP
                          : Mdi::CHEVRON_DOWN,
                    24
                )
            ),
            state ? insets(
                        {0, 0, 0, 0},
                        child()
                    ) | slideIn(Ui::SlideFrom::TOP) |
                        Ui::grow()
                  : Ui::empty()
        );
    });
}

Ui::Child treeRow(Opt<Ui::Slot> leading, String title, Opt<String> subtitle, Ui::Slots children) {
    Ui::Slot slot = [children = std::move(children)] {
        return vflow(children());
    };
    return treeRow(std::move(leading), title, subtitle, std::move(slot));
}

} // namespace Karm::Kira
