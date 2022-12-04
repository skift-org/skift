#pragma once

#include "input.h"
#include "layout.h"
#include "react.h"
#include "view.h"

namespace Karm::Ui {

inline Child row(Child child) {
    return minSize(
        {UNCONSTRAINED, 64},
        spacing(
            {16},
            Ui::vcenter(child)));
}

inline Child row(Opt<Child> leading, String title, Opt<String> subtitle, Opt<Child> trailing) {
    auto lead = leading
                    ? spacing({0, 0, 12, 0}, minSize(26, center(*leading)))
                    : empty();

    auto t = subtitle
                 ? vflow(
                       8,
                       text(TextStyle::labelLarge(), title),
                       text(TextStyle::labelMedium(), *subtitle))
                 : text(TextStyle::labelLarge(), title);

    auto trail = trailing
                     ? spacing({12, 0, 0, 0}, minSize(26, center(*trailing)))
                     : empty();

    return minSize(
        {UNCONSTRAINED, 48},
        spacing(
            {12, 0},
            hflow(
                0,
                Layout::Align::VCENTER | Layout::Align::HFILL,
                lead,
                grow(t),
                trail)));
}

inline Child titleRow(String t) {
    return spacing({12, 16, 12, 8}, text(Ui::TextStyle::titleMedium(), t));
}

inline Child pressableRow(OnPress onPress, Opt<Child> leading, String title, Opt<String> subtitle, Opt<Child> trailing) {
    return button(
        std::move(onPress),
        ButtonStyle::subtle(),
        row(
            leading,
            title,
            subtitle,
            trailing));
}

inline Child buttonRow(OnPress onPress, Media::Icons i, String title, String subtitle) {
    return button(
        std::move(onPress),
        ButtonStyle::subtle(),
        row(
            icon(i, 24),
            title,
            subtitle,
            NONE));
}

inline Child buttonRow(OnPress onPress, String title, String text) {
    return row(
        NONE,
        title,
        NONE,
        button(std::move(onPress), ButtonStyle::primary(), text));
}

inline Child toggleRow(bool value, OnChange<bool> onChange, String title) {
    return row(
        NONE,
        title,
        NONE,
        toggle(value, std::move(onChange)));
}

inline Child checkboxRow(bool value, OnChange<bool> onChange, String title) {
    return row(
        NONE,
        title,
        NONE,
        checkbox(value, std::move(onChange)));
}

inline Child radioRow(bool value, OnChange<bool> onChange, String title) {
    return row(
        radio(value, std::move(onChange)),
        title,
        NONE,
        NONE);
}

inline Child sliderRow(double value, OnChange<double> onChange, String title) {
    return row(
        NONE,
        title,
        NONE,
        slider(value, std::move(onChange)));
}

inline Child navRow(bool selected, OnPress onPress, Media::Icons i, String title) {
    auto buttonStyle = ButtonStyle::regular();
    buttonStyle.idleStyle = {
        .borderRadius = 4,
        .backgroundColor = selected ? Gfx::ZINC800 : Gfx::ALPHA,
    };

    auto indicator = box(BoxStyle{
                             .borderRadius = 99,
                             .backgroundColor = selected ? Gfx::BLUE600 : Gfx::ALPHA,
                         },
                         empty(4));

    return button(
        std::move(onPress),
        buttonStyle,
        spacing(
            {0, 8, 12, 8},
            hflow(
                indicator,
                empty(8),
                icon(i, 26),
                empty(12),
                center(text(TextStyle::labelMedium(), title)))));
}

inline Child treeRow(Opt<Child> leading, String title, Opt<String> subtitle, Children children) {
    return state(false, [=](State<bool> state) {
        return vflow(
            0,
            pressableRow(
                state.bindToggle(),
                leading,
                title,
                subtitle,
                icon(state.value() ? Media::Icons::CHEVRON_UP : Media::Icons::CHEVRON_DOWN, 24)),
            state.value() ? spacing(
                                {38, 0, 0, 0},
                                vflow(children))
                          : empty());
    });
}

} // namespace Karm::Ui
