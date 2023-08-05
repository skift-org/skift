#include <karm-ui/box.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>

#include "app.h"

namespace Shell {

struct QuickSettingProps {
    Mdi::Icon icon;
    Opt<String> name;
    bool state;
    Ui::OnPress press;
};

Ui::Child quickSetting(QuickSettingProps props) {
    auto style = (props.state
                      ? Ui::ButtonStyle::primary()
                      : Ui::ButtonStyle::secondary());

    if (props.name) {
        return Ui::button(std::move(props.press), style, props.icon, *props.name);
    } else {
        return Ui::button(std::move(props.press), style, props.icon);
    }
}

Ui::Child quickSettingSlider(Mdi::Icon icon) {
    return Ui::hflow(Ui::grow(NONE), Ui::icon(icon) | Ui::center() | Ui::aspectRatio(1) | Ui::bound() | Ui::dragRegion()) |
           Ui::box({
               .borderRadius = 6,
               .backgroundPaint = Ui::ACCENT600,
           }) |
           Ui::slider2(0.5, NONE) |
           Ui::box({
               .borderRadius = 6,
               .backgroundPaint = Ui::GRAY900,
           }) |
           Ui::maxSize({Ui::UNCONSTRAINED, 36});
}

Ui::Child dateAndTime() {
    return Ui::vflow(
        Ui::labelLarge("22:07"),
        Ui::labelMedium("Fri, Jul 28"));
}

Ui::Child quickheader(State const &state) {
    return Ui::hflow(
        Ui::empty(6),
        dateAndTime() | Ui::center(),
        Ui::grow(NONE),
        Ui::button(
            Model::bind<ToggleSysPanel>(),
            Ui::ButtonStyle::secondary(),
            state.isSysPanelColapsed
                ? Mdi::CHEVRON_DOWN
                : Mdi::CHEVRON_UP));
}

Ui::Child quickTools(State const &) {
    return Ui::hflow(
        8,
        Ui::button(
            Model::bind<Lock>(),
            Ui::ButtonStyle::secondary(),
            Mdi::LOCK),
        Ui::button(
            [](auto &n) {
                Model::dispatch(n, Activate{Panel::NIL});
                Ui::showDialog(n, powerDialog());
            },
            Ui::ButtonStyle::secondary(), Mdi::POWER),
        Ui::grow(NONE),
        Ui::button(
            Model::bind<ToggleSysPanel>(),
            Ui::ButtonStyle::secondary(),
            Mdi::COG));
}

Ui::Child colapsedQuickSettings(State const &) {
    auto settings = Ui::grid(
        {
            .rows = Ui::GridUnit::fixed(46).repeated(2),
            .columns = Ui::GridUnit::grow().repeated(2),
            .gaps = 8,
        },
        quickSetting({
            .icon = Mdi::SWAP_VERTICAL,
            .name = "Cellular Data",
            .press = Ui::NOP,
        }),
        quickSetting({
            .icon = Mdi::WIFI_STRENGTH_4,
            .name = "Wi-Fi",
            .press = Ui::NOP,
        }),
        quickSetting({
            .icon = Mdi::BLUETOOTH,
            .name = "Bluetooth",
            .press = Ui::NOP,
        }),
        quickSetting({
            .icon = Mdi::FLASHLIGHT,
            .name = "Flashlight",
            .press = Ui::NOP,
        }));

    return settings;
}

Ui::Child expendedQuickSettings(State const &state) {
    auto settings = Ui::grid(
        {
            .rows = Ui::GridUnit::fixed(46).repeated(4),
            .columns = Ui::GridUnit::grow().repeated(2),
            .gaps = 8,
        },
        quickSetting({
            .icon = Mdi::SWAP_VERTICAL,
            .name = "Cellular Data",
            .press = Ui::NOP,
        }),
        quickSetting({
            .icon = Mdi::WIFI_STRENGTH_4,
            .name = "Wi-Fi",
            .press = Ui::NOP,
        }),
        quickSetting({
            .icon = Mdi::BLUETOOTH,
            .name = "Bluetooth",
            .press = Ui::NOP,
        }),
        quickSetting({
            .icon = Mdi::FLASHLIGHT,
            .name = "Flashlight",
            .press = Ui::NOP,
        }),
        quickSetting({
            .icon = Mdi::MAP_MARKER_OUTLINE,
            .name = "Location",
            .press = Ui::NOP,
        }),
        quickSetting({
            .icon = state.isMobile ? Mdi::LAPTOP : Mdi::CELLPHONE,
            .name = "Tablet Mode",
            .state = state.isMobile,
            .press = Model::bind<ToggleTablet>(),
        }),
        quickSetting({
            .icon = Mdi::CIRCLE_HALF_FULL,
            .name = "Dark Mode",
            .press = Ui::NOP,
        }));

    return Ui::vflow(
        8,
        quickSettingSlider(Mdi::BRIGHTNESS_6),
        quickSettingSlider(Mdi::VOLUME_HIGH),
        settings | Ui::grow(),
        quickTools(state));
}

Ui::Child quickSettings(State const &state) {
    if (state.isSysPanelColapsed) {
        return colapsedQuickSettings(state);
    } else {
        return expendedQuickSettings(state);
    }
}

Ui::Child notiWrapper(App app, Ui::Child inner) {
    return Ui::vflow(
               8,
               Ui::hflow(
                   4,
                   Ui::box({.foregroundPaint = app.color[4]},
                           Ui::icon(app.icon, 12)),
                   Ui::text(Ui::TextStyle::labelMedium().withColor(Ui::GRAY400), app.name)),
               inner) |
           Ui::box({
               .padding = 12,
               .borderRadius = 4,
               .backgroundPaint = Ui::GRAY900,
           });
}

Ui::Child notiMsg(String title, String body) {
    return Ui::vflow(
        6,
        Ui::hflow(Ui::text(Ui::TextStyle::labelLarge(), title)),
        Ui::text(Ui::TextStyle::labelMedium(), body));
}

Ui::Child notification(Mdi::Icon icon, String title, String subtitle) {
    return notiWrapper({icon, Gfx::BLUE_RAMP, "Hello, world"}, notiMsg(title, subtitle));
}

Ui::Child notifications() {
    return Ui::vflow(
        8,
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"),
        notification(Mdi::HAND_WAVE, "Hello", "Hello, world!"));
}

Ui::Child sysPanel(State const &state) {
    return expendedQuickSettings(state) | panel({320, Ui::UNCONSTRAINED});
}

Ui::Child notiPanel(State const &) {
    return notifications() | panel();
}

Ui::Child sysFlyout(State const &state) {
    auto box = Ui::box({
        .margin = {8, 8, 8, 32},
        .padding = {12, 12, 12, 0},
        .borderRadius = 8,
        .borderWidth = 1,
        .borderPaint = Ui::GRAY800,
        .backgroundPaint = Ui::GRAY950,
    });

    Ui::Children body;
    body.pushBack(quickheader(state));
    if (state.isSysPanelColapsed){
        body.pushBack(colapsedQuickSettings(state));
        body.pushBack(notifications() | Ui::grow());
    }else{
        body.pushBack(expendedQuickSettings(state) | Ui::grow());
    }
    body.pushBack(Ui::dragHandle());

    return Ui::vflow(8,body) |
           box |
           Ui::bound() |
           Ui::dismisable(
               Model::bind<Activate>(Panel::NIL),
               Ui::DismisDir::TOP,
               0.3) |
           Ui::slideIn(Ui::SlideFrom::TOP);
}

} // namespace Shell
