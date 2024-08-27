#include <karm-ui/anim.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>

#include "checkbox.h"
#include "color-input.h"
#include "radio.h"
#include "row.h"
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
                          Ui::insets({0, 0, 12, 0})
                    : Ui::empty();

    auto t = subtitle
                 ? Ui::vflow(
                       8,
                       Ui::labelLarge(title),
                       Ui::labelMedium(*subtitle)
                   )
                 : Ui::labelLarge(title);

    auto trail = trailing
                     ? *trailing |
                           Ui::center() |
                           Ui::sizing(26, {Ui::UNCONSTRAINED, 26}) |
                           Ui::insets({0, 0, 12, 0})
                     : Ui::empty();

    return minSize(
        {Ui::UNCONSTRAINED, 48},
        insets(
            {12, 0},
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
           Ui::insets({12, 16, 12, 8});
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

Ui::Child colorRow(Gfx::Color c, Ui::OnChange<Gfx::Color> onChange, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        Kr::colorInput(c, std::move(onChange))
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
                        {38, 0, 0, 0},
                        child()
                    ) | slideIn(Ui::SlideFrom::TOP)
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
