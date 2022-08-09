#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/box.h>
#include <karm-ui/dock.h>
#include <karm-ui/empty.h>
#include <karm-ui/flow.h>
#include <karm-ui/host.h>

CliResult entryPoint(CliArgs args) {
    auto emptyBox = [](auto dock, auto color) {
        return Ui::docked(dock, Ui::box(color, Ui::empty(64)));
    };

    auto toolbar = [](auto... children) {
        return Ui::docked(
            Layout::Dock::TOP,
            Ui::box(Gfx::ZINC800, Ui::hflow(children...)));
    };
    return Ui::runApp(args,
                      Ui::dock(
                          toolbar(
                              Ui::box(Gfx::INDIGO500, Ui::empty(64)),
                              Ui::box(Gfx::AMBER500, Ui::empty(64)),
                              Ui::box(Gfx::CYAN500, Ui::empty(64))),
                          emptyBox(Layout::Dock::BOTTOM, Gfx::BLUE500),
                          emptyBox(Layout::Dock::START, Gfx::GREEN500),
                          emptyBox(Layout::Dock::END, Gfx::YELLOW500),
                          emptyBox(Layout::Dock::FILL, Gfx::PINK500)));
}
