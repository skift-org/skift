#include <karm-sys/entry.h>
#include <karm-ui/app.h>

#include "../app.h"

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::About::app());
}
