#include <karm-app/form-factor.h>
#include <karm-image/loader.h>
#include <karm-sys/entry.h>
#include <karm-sys/time.h>
#include <karm-ui/app.h>
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

#include "../app.h"
#include "../mock.h"

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto args = useArgs(ctx);
    bool isMobile = App::useFormFactor() == App::FormFactor::MOBILE;

    Hideo::Shell::State state = {
        .isMobile = isMobile,
        .dateTime = Sys::dateTime(),
        .background = co_try$(Image::loadOrFallback("bundle://hideo-shell/wallpapers/winter.qoi"_url)),
        .noti = {},
        .launchers = {
            makeRc<Hideo::Shell::MockLauncher>(Mdi::INFORMATION_OUTLINE, "About"s, Gfx::BLUE_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::CALCULATOR, "Calculator"s, Gfx::ORANGE_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::PALETTE_SWATCH, "Color Picker"s, Gfx::RED_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::COUNTER, "Counter"s, Gfx::GREEN_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::DUCK, "Demos"s, Gfx::YELLOW_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::FILE, "Files"s, Gfx::ORANGE_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::FORMAT_FONT, "Fonts"s, Gfx::BLUE_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::EMOTICON, "Hello World"s, Gfx::RED_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::IMAGE, "Icons"s, Gfx::GREEN_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::IMAGE, "Image Viewer"s, Gfx::YELLOW_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::COG, "Settings"s, Gfx::ZINC_RAMP),
            makeRc<Hideo::Shell::MockLauncher>(Mdi::TABLE, "Spreadsheet"s, Gfx::GREEN_RAMP),
        },
        .instances = {}
    };

    co_return Ui::runApp(ctx, Hideo::Shell::app(std::move(state)));
}
