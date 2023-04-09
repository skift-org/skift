#include <json/json.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>
#include <karm-ui/app.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>

#include "loader.h"

namespace Loader {

Ui::Child icon(Entry const &e) {
    auto maybeImage = Media::loadImage(e.icon);
    if (maybeImage) {
        return Ui::pinSize(64, Ui::image(maybeImage.unwrap()));
    }
    return Ui::icon(Mdi::byName(e.icon).unwrap(), 64);
}

Ui::Child entry(Entry const &e) {
    return Ui::vflow(
               16,
               Layout::Align::CENTER,
               icon(e),
               Ui::headlineSmall(e.name)) |
           Ui::center() |
           Ui::pinSize({192, 128}) |
           Ui::spacing(16) | Ui::button(Ui::NOP, Ui::ButtonStyle::secondary());
}

Ui::Child list(Configs const &c) {
    return Ui::hflow(
        16,
        iter(c.entries).map(entry).collect<Vec<Ui::Child>>());
}

Ui::Child menu(Configs const &c) {
    return Ui::vflow(
               16,
               Layout::Align::CENTER,
               Ui::titleLarge("Select an operating system"),
               list(c) | Ui::grow(4),
               Ui::bodyMedium("Use the arrow keys to navigate, and press enter to select an entry.")) |
           Ui::spacing(64);
}

} // namespace Loader

Res<> entryPoint(CliArgs args) {
    logInfo("");
    logInfo("  _                 _");
    logInfo(" | |   ___  __ _ __| |___ _ _");
    logInfo(" | |__/ _ \\/ _` / _` / -_) '_|");
    logInfo(" |____\\___/\\__,_\\__,_\\___|_|");
    logInfo("");

    logInfo("Loading configs...");
    auto file = try$(Sys::File::open("/boot/loader.json"));

    logInfo("Parsing configs...");
    auto fileStr = try$(Io::readAllUtf8(file));
    auto json = try$(Json::parse(fileStr));

    logInfo("Validating configs...");
    logInfo("Configs: {}", json);

    auto configs = try$(Loader::Configs::fromJson(json));

    if (configs.entries.len() > 1) {
        return Ui::runApp(args, Loader::menu(configs));
    } else {
        return Loader::load(configs.entries[0]);
    }
}
