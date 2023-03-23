#include <karm-main/main.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scafold.h>
#include <karm-ui/view.h>

auto NOP(Ui::Node &) {}

Ui::Child toolbar() {
    return Ui::toolbar(
        Ui::button(NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::MAGNIFY_PLUS),
        Ui::button(NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::MAGNIFY_MINUS),
        Ui::button(NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::FULLSCREEN),
        Ui::grow(NONE),
        Ui::button(NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::PENCIL, "Edit"));
}

Ui::Child controls() {
    return Ui::hflow(
               Ui::button(NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::ARROW_LEFT),
               Ui::button(NOP, Ui::ButtonStyle::subtle(), Mdi::Icon::ARROW_RIGHT)) |
           Ui::spacing({0, 0, 0, 8}) |
           Ui::center();
}

Res<> entryPoint(CliArgs args) {
    auto titlebar = Ui::titlebar(Mdi::IMAGE, "Image Viewer");

    auto content =
        Ui::image(try$(Media::loadImage(args[0])), 8) |
        Ui::spacing(8) |
        Ui::fit();

    auto layout = Ui::vflow(
                      titlebar,
                      toolbar(),
                      content | Ui::grow(),
                      controls()) |
                  Ui::maxSize({700, 500}) |
                  Ui::dialogLayer();

    return Ui::runApp(args, layout);
}
