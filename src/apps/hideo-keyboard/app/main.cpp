#include <karm-main/main.h>
#include <karm-ui/app.h>

#include "../views.h"

Res<> entryPoint(Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Keyboard::flyout());
}
