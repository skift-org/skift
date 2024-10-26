#include <karm-kira/error-page.h>
#include <karm-kira/scaffold.h>
#include <karm-kira/toolbar.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <mdi/alert-decagram.h>
#include <mdi/arrow-left.h>
#include <mdi/arrow-right.h>
#include <mdi/fullscreen.h>
#include <mdi/magnify-minus.h>
#include <mdi/magnify-plus.h>
#include <mdi/pencil.h>

#include "app.h"

namespace Hideo::Images {

Ui::Child viewer(State const &state) {
    return Ui::vflow(
        viewerToolbar(state),
        viewerPreview(state) | Ui::grow(),
        viewerControls(state)
    );
}

Ui::Child viewerPreview(State const &state) {
    if (not state.image)
        return Kr::errorPage(Mdi::ALERT_DECAGRAM, "No Image"s, "Unable to display this image."s);

    return Ui::image(state.image.unwrap()) |
           Ui::box({
               .borderWidth = 1,
               .borderFill = Ui::GRAY50.withOpacity(0.1),
           }) |
           Ui::insets(8) |
           Ui::fit();
}

Ui::Child viewerToolbar(State const &state) {
    return Kr::toolbar({
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::MAGNIFY_PLUS),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::MAGNIFY_MINUS),
        Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::FULLSCREEN),
        Ui::grow(NONE),
        Ui::button(
            Model::bindIf<ToggleEditor>(state.image.has()),
            Ui::ButtonStyle::subtle(),
            Mdi::PENCIL,
            "Edit"
        ),
    });
}

Ui::Child viewerControls(State const &) {
    return Ui::hflow(
               Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
               Ui::button(Ui::NOP, Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT)
           ) |
           Ui::insets({0, 0, 8, 0}) |
           Ui::center();
}

} // namespace Hideo::Images
