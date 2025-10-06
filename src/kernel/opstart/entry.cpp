import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

#include <karm-sys/entry.h>

#include "loader.h"

void drawSplash(Gfx::Canvas& g, Opstart::Entry const& e, Math::Recti size) {
    if (auto it = e.icon.is<Rc<Gfx::Surface>>()) {
        g.clear(Gfx::ZINC900);
        auto dest = (*it)->bound().center(size);
        g.blit(dest, (*it)->pixels());
    }
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
