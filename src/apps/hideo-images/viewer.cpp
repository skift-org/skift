export module Hideo.Images:viewer;

import Mdi;
import Karm.Core;
import Karm.Kira;
import Karm.Ui;
import :model;

namespace Hideo::Images {

Ui::Child viewerPreview(State const& state) {
    if (not state.image)
        return Kr::errorPage(Mdi::ALERT_DECAGRAM, "Unable to display this image."s, Str{state.image.none().msg()});

    return Ui::image(state.image.unwrap()) |
           Ui::box({
               .borderWidth = 1,
               .borderFill = Ui::GRAY50.withOpacity(0.1),
           }) |
           Ui::insets(8) |
           Ui::fit();
}

Ui::Child viewerToolbar(State const& state) {
    return Kr::toolbar({
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::MAGNIFY_PLUS),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::MAGNIFY_MINUS),
        Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FULLSCREEN),
        Ui::grow(NONE),
        Ui::button(
            Model::bindIf<ToggleEditor>(state.image.has()),
            Ui::ButtonStyle::subtle(),
            Mdi::PENCIL,
            "Edit"
        ),
    });
}

Ui::Child viewerControls(State const&) {
    return Ui::hflow(
               Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
               Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT)
           ) |
           Ui::insets({0, 0, 8, 0}) |
           Ui::center();
}

export Ui::Child viewerApp(State const& state) {
    return Kr::scaffold({
        .icon = Mdi::IMAGE,
        .title = "Images"s,
        .startTools = [&] -> Ui::Children {
            return {
                Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::MAGNIFY_PLUS),
                Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::MAGNIFY_MINUS),
                Ui::button(Ui::SINK<>, Ui::ButtonStyle::subtle(), Mdi::FULLSCREEN),
            };
        },
        .endTools = [&] -> Ui::Children {
            return {
                Ui::button(
                    Model::bindIf<ToggleEditor>(state.image.has()),
                    Ui::ButtonStyle::subtle(),
                    Mdi::PENCIL,
                    "Edit"
                ),
            };
        },
        .body = [&] {
            return Ui::vflow(
                viewerPreview(state) | Ui::grow(),
                viewerControls(state)
            );
        },
    });
}

} // namespace Hideo::Images
