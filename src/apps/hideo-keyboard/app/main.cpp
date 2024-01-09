#include <karm-sys/entry.h>
#include <karm-ui/app.h>

#include "../views.h"

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Keyboard::flyout());
}
