import Karm.Gfx;

#include <karm-logger/logger.h>
#include <karm-math/align.h>
#include <karm-math/au.h>
#include <karm-sys/entry.h>

#include "karm-math/align.h"
#include "loader.h"

void drawSplash(Gfx::Canvas& g, Opstart::Entry const& e, Math::Recti size) {
    g.clear(Gfx::ZINC950);
    if (auto it = e.icon.is<Rc<Gfx::Surface>>()) {
        auto dest = Math::Align{
            Math::Align::BOTTOM | Math::Align::HCENTER,
        }
                        .apply(Math::Flow::LEFT_TO_RIGHT, (*it)->bound(), size.vsplit(size.height / 2).v0.shrink(32));
        g.blit(dest, (*it)->pixels());
    } else {
        g.push();
        Gfx::Prose prose{
            {
                .font = Ui::TextStyles::headlineLarge().font,
            },
            e.name
        };
        Math::Recti proseSize = prose.layout(Au{size.width}).cast<isize>();

        g.translate(
            Math::Align{
                Math::Align::BOTTOM | Math::Align::HCENTER,
            }
                .apply(Math::Flow::LEFT_TO_RIGHT, proseSize, size.vsplit(size.height / 2).v0.shrink(32))
                .topStart()
                .cast<f64>()
        );

        g.fill(prose);
        g.pop();
    }

    g.push();
    Gfx::Prose prose{
        {
            .font = Ui::TextStyles::labelLarge().font,
        },
        "Press F8 for boot options"
    };
    Math::Recti proseSize = prose.layout(Au{size.width}).cast<isize>();

    g.translate(
        Math::Align{
            Math::Align::BOTTOM | Math::Align::HCENTER,
        }
            .apply(Math::Flow::LEFT_TO_RIGHT, proseSize, size.shrink(32))
            .topStart()
            .cast<f64>()
    );

    g.fill(prose);
    g.pop();
}

Res<> splashScreen(Opstart::Entry const& e) {
    auto* gop = try$(Efi::locateProtocol<Efi::GraphicsOutputProtocol>());
    auto* mode = gop->mode;
    logInfo("efi: gop: {}x{}, {} stride, {} modes", mode->info->horizontalResolution, mode->info->verticalResolution, mode->info->pixelsPerScanLine * 4, mode->maxMode);
    Gfx::MutPixels front = {
        (void*)mode->frameBufferBase,
        {(u16)mode->info->horizontalResolution, (u16)mode->info->verticalResolution},
        (u16)(mode->info->pixelsPerScanLine * 4),
        Gfx::BGRA8888,
    };

    Gfx::CpuCanvas g;
    g.begin(front);
    drawSplash(g, e, front.bound());
    g.end();

    return Ok();
}

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    logInfo("opstart " stringify$(__ck_version_value));

    logInfo("loading configs...");
    auto file = co_try$(Sys::File::open("file:/boot/loader.json"_url));

    logInfo("parsing configs...");
    auto fileStr = co_try$(Io::readAllUtf8(file));
    auto json = co_try$(Json::parse(fileStr));

    logInfo("validating configs...");
    logInfo("configs: {}", json);

    auto configs = co_try$(Opstart::Configs::fromJson(json));

    if (configs.entries.len() > 1 or configs.entries.len() == 0)
        co_return co_await Opstart::showMenuAsync(ctx, configs);
    else
        co_try$(splashScreen(configs.entries[0]));

    co_return Opstart::loadEntry(configs.entries[0]);
}
