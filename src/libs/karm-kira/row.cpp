module;

#include <karm-gfx/icon.h>
#include <karm-math/align.h>

export module Karm.Kira:row;

import Karm.Ui;
import Mdi;
import :checkbox;
import :colorInput;
import :number;
import :radio;
import :select;
import :slider;
import :toggle;

namespace Karm::Kira {

export Ui::Child rowContent(Ui::Child child) {
    return child |
           Ui::vcenter() |
           Ui::insets(16) |
           Ui::minSize({Ui::UNCONSTRAINED, 64});
}

export Ui::Child rowContent(Opt<Ui::Child> leading, String title, Opt<String> subtitle, Opt<Ui::Child> trailing) {
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

export Ui::Child titleRow(String t) {
    return Ui::titleMedium(t) |
           Ui::insets({16, 12, 8, 12});
}

export Ui::Child pressableRow(Opt<Ui::Send<>> onPress, Opt<Ui::Child> leading, String title, Opt<String> subtitle, Opt<Ui::Child> trailing) {
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

export Ui::Child buttonRow(Opt<Ui::Send<>> onPress, Gfx::Icon i, String title, String subtitle) {
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

export Ui::Child buttonRow(Opt<Ui::Send<>> onPress, String title, String text) {
    return rowContent(
        NONE,
        title,
        NONE,
        button(std::move(onPress), Ui::ButtonStyle::primary(), text)
    );
}

export Ui::Child toggleRow(bool value, Ui::Send<bool> onChange, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        toggle(value, std::move(onChange))
    );
}

export Ui::Child checkboxRow(bool value, Ui::Send<bool> onChange, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        checkbox(value, std::move(onChange))
    );
}

export Ui::Child radioRow(bool value, Ui::Send<bool> onChange, String title) {
    return rowContent(
        radio(value, std::move(onChange)),
        title,
        NONE,
        NONE
    );
}

export Ui::Child sliderRow(f64 value, Ui::Send<f64> onChange, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        slider(
            value,
            std::move(onChange),
            Mdi::DRAG_VERTICAL_VARIANT,
            ""
        ) | Ui::minSize({128, Ui::UNCONSTRAINED})
    );
}

export Ui::Child selectRow(Ui::Child value, Ui::Slots options, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        select(std::move(value), std::move(options))
    );
}

export Ui::Child colorRow(Gfx::Color c, Ui::Send<Gfx::Color> onChange, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        colorInput(c, std::move(onChange))
    );
}

export Ui::Child numberRow(f64 value, Ui::Send<f64> onChange, f64 step, String title) {
    return rowContent(
        NONE,
        title,
        NONE,
        number(value, std::move(onChange), step)
    );
}

export Ui::Child treeRow(Opt<Ui::Slot> leading, String title, Opt<String> subtitle, Ui::Slot child) {
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

export Ui::Child treeRow(Opt<Ui::Slot> leading, String title, Opt<String> subtitle, Ui::Slots children) {
    Ui::Slot slot = [children = std::move(children)] {
        return vflow(children());
    };
    return treeRow(std::move(leading), title, subtitle, std::move(slot));
}

} // namespace Karm::Kira
