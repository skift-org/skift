#include <handover/hook.h>
#include <hjert-api/api.h>

#include "host.h"

namespace Grund::Shell {

Res<Gfx::MutPixels> openFramebuffer(Sys::Context &ctx) {
    auto &handover = useHandover(ctx);

    auto *fb = handover.findTag(Handover::Tag::FB);
    auto fbVmo = try$(Hj::Vmo::create(Hj::ROOT, fb->start, fb->size, Hj::VmoFlags::DMA));
    try$(fbVmo.label("framebuffer"));

    static auto fbRange = try$(Hj::map(fbVmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    logInfo("fb: {x}-{x} {}x{}, {} stride", fbRange.range().start, fbRange.range().end(), fb->fb.width, fb->fb.height, fb->fb.pitch);

    return Ok(Gfx::MutPixels{
        fbRange.mutBytes().buf(),
        {fb->fb.width, fb->fb.height},
        fb->fb.pitch,
        Gfx::BGRA8888,
    });
}

Res<Strong<RootHost>> makeHost(Sys::Context &ctx, Ui::Child root) {
    Gfx::MutPixels front = try$(openFramebuffer(ctx));
    auto back = Gfx::Surface::alloc(
        front.size(), Gfx::BGRA8888
    );

    return Ok(makeStrong<RootHost>(std::move(root), front, back));
}

} // namespace Grund::Shell
