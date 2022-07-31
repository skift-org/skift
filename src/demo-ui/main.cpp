#include <karm-main/main.h>
#include <karm-ui/node.h>

ExitCode entryPoint(CliArgs const &) {
    auto emptyBox = [](auto dock, auto color) {
        return Ui::docked(dock, Ui::box(color, Ui::empty(10)));
    };

    return Ui::show(
        Ui::dock(
            emptyBox(Ui::Dock::TOP, Gfx::RED500),
            emptyBox(Ui::Dock::BOTTOM, Gfx::BLUE500),
            emptyBox(Ui::Dock::START, Gfx::GREEN500),
            emptyBox(Ui::Dock::END, Gfx::YELLOW500),
            emptyBox(Ui::Dock::FILL, Gfx::PINK500)));
}
