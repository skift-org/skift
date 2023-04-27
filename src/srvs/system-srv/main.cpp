#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-gfx/context.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>

Res<> entryPoint(Ctx &ctx) {
    try$(Hj::Task::self().label("system"));

    auto &handover = useHandover(ctx);

    auto *fb = handover.findTag(Handover::Tag::FB);
    auto fbVmo = try$(Hj::createVmo(Hj::ROOT, fb->start, fb->size, Hj::VmoFlags::DMA));
    try$(fbVmo.label("framebuffer"));
    auto fbAddr = try$(Hj::Space::self().map(fbVmo, Hj::MapFlags::READ | Hj::MapFlags::WRITE));

    Gfx::MutPixels pixels = {
        (void *)fbAddr,
        {fb->fb.width, fb->fb.height},
        fb->fb.pitch,
        Gfx::BGRA8888,
    };

    pixels.clear(Gfx::GREEN);
    pixels.clip(pixels.bound().shrink(10)).clear(Gfx::ZINC900);

    logInfo("Hello from system server!");
    return Ok();
}
