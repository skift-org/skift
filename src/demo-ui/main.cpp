#include <karm-main/main.h>
#include <karm-ui/node.h>

ExitCode entryPoint(CliArgs const &) {
    auto emptyBox = [](auto dock, auto color) {
        return Ui::docked(dock, Ui::box(color, Ui::empty(64)));
    };

    auto toolbar = [](auto... children) {
        return Ui::docked(Ui::Dock::TOP, Ui::box(Gfx::ZINC800, Ui::flow(children...)));
    };

    return Ui::show(
        Ui::dock(
            toolbar(
                Ui::box(Gfx::INDIGO500, Ui::empty(64)),
                Ui::box(Gfx::AMBER500, Ui::empty(64)),
                Ui::box(Gfx::CYAN500, Ui::empty(64))),
            emptyBox(Ui::Dock::BOTTOM, Gfx::BLUE500),
            emptyBox(Ui::Dock::START, Gfx::GREEN500),
            emptyBox(Ui::Dock::END, Gfx::YELLOW500),
            emptyBox(Ui::Dock::FILL, Gfx::PINK500)));
}
