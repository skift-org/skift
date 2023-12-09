#include <karm-ui/input.h>
#include <karm-ui/scafold.h>

#include "app.h"

namespace ImageViewer {

Ui::Child alert(String title, String subtitle) {
    return Ui::vflow(
               16,
               Layout::Align::CENTER,
               Ui::icon(Mdi::ALERT_DECAGRAM, 48),
               Ui::titleLarge(title),
               Ui::bodyMedium(subtitle)) |
           Ui::box({
               .foregroundPaint = Ui::GRAY500,
           }) |
           Ui::center();
}

Ui::Child viewer(State const &state) {
    return Ui::vflow(
        viewerToolbar(state),
        viewerPreview(state) | Ui::grow(),
        viewerControls(state));
}

Ui::Child viewerPreview(State const &state) {
    if (not state.image) {
        return alert("Unable to display this image", state.image.none().msg());
    }

    return Ui::image(state.image.unwrap()) |
           Ui::box({
               .borderWidth = 1,
               .borderPaint = Ui::GRAY50.withOpacity(0.1),
           }) |
           Ui::spacing(8) |
           Ui::fit();
}

Ui::Child viewerToolbar(State const &state) {
    return Ui::toolbar(
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::MAGNIFY_PLUS),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::MAGNIFY_MINUS),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::FULLSCREEN),
        Ui::grow(NONE),
        Ui::button(
            Model::bindIf<ToggleEditor>(state.image.has()),
            Ui::ButtonStyle::subtle(),
            Mdi::Icon::PENCIL,
            "Edit"));
}

Ui::Child viewerControls(State const &) {
    return Ui::hflow(
               Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::ARROW_LEFT),
               Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::ARROW_RIGHT)) |
           Ui::spacing({0, 0, 0, 8}) |
           Ui::center();
}

} // namespace ImageViewer
