module;

#include <karm/macros>
#include <vaerk-efi/base.h>

export module Opstart:splash;

import Karm.Gfx;
import Karm.Logger;
import Karm.Math;

import :config;

using namespace Karm;

namespace Opstart {

void drawSplash(Gfx::Canvas& g, Opstart::Entry const& e, Math::Recti size) {
    if (auto it = e.icon.is<Rc<Gfx::Surface>>()) {
        g.clear(Gfx::ZINC950);
        auto dest = (*it)->bound().center(size);
        g.blit(dest, (*it)->pixels());
    }
}

export Res<> splashScreen(Opstart::Entry const& e) {
    auto* gop = try$(Efi::locateProtocol<Efi::GraphicsOutputProtocol>());
    auto* mode = gop->mode;

    Gfx::MutPixels framebuffer = {
        reinterpret_cast<void*>(mode->frameBufferBase),
        {
            static_cast<u16>(mode->info->horizontalResolution),
            static_cast<u16>(mode->info->verticalResolution),
        },
        static_cast<u16>(mode->info->pixelsPerScanLine * 4),
        Gfx::BGRA8888,
    };

    Gfx::CpuCanvas g;
    g.begin(framebuffer);
    drawSplash(g, e, framebuffer.bound());
    g.end();

    return Ok();
}

} // namespace Opstart
