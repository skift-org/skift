#include <hideo-shell/app.h>
#include <karm-image/loader.h>
#include <karm-sys/entry.h>
#include <mdi/calculator.h>
#include <mdi/cog.h>
#include <mdi/counter.h>
#include <mdi/duck.h>
#include <mdi/emoticon.h>
#include <mdi/file.h>
#include <mdi/format-font.h>
#include <mdi/image.h>
#include <mdi/information-outline.h>
#include <mdi/palette-swatch.h>
#include <mdi/table.h>
#include <mdi/widgets.h>

#include "host.h"

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    Hideo::Shell::State state = {
        .isMobile = false,
        .dateTime = Sys::dateTime(),
        .background = co_try$(Image::loadOrFallback("bundle://hideo-shell/wallpaper.qoi"_url)),
        .noti = {},
        .manifests = {
            makeStrong<Hideo::Shell::Manifest>(Mdi::INFORMATION_OUTLINE, "About"s, Gfx::BLUE_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::CALCULATOR, "Calculator"s, Gfx::ORANGE_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::PALETTE_SWATCH, "Color Picker"s, Gfx::RED_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::COUNTER, "Counter"s, Gfx::GREEN_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::DUCK, "Demos"s, Gfx::YELLOW_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::FILE, "Files"s, Gfx::ORANGE_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::FORMAT_FONT, "Fonts"s, Gfx::BLUE_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::EMOTICON, "Hello World"s, Gfx::RED_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::IMAGE, "Icons"s, Gfx::GREEN_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::IMAGE, "Image Viewer"s, Gfx::YELLOW_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::COG, "Settings"s, Gfx::ZINC_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::TABLE, "Spreadsheet"s, Gfx::GREEN_RAMP),
            makeStrong<Hideo::Shell::Manifest>(Mdi::WIDGETS, "Widget Gallery"s, Gfx::BLUE_RAMP),
        },
        .instances = {}
    };

    auto app = Hideo::Shell::app(std::move(state));
    auto host = co_try$(Grund::Shell::makeHost(ctx, app));
    co_return host->run();
}
