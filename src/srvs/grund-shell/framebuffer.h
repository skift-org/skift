#pragma once

#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-gfx/cpu/surface.h>
#include <karm-logger/logger.h>

namespace Grund::Shell {

struct Framebuffer : public Gfx::CpuSurface {
    static Res<Rc<Framebuffer>> open(Sys::Context &ctx);

    Handover::Record _record;
    Hj::Mapped _map;

    Framebuffer(Handover::Record record, Hj::Mapped map);

    Gfx::MutPixels mutPixels();
};

} // namespace Grund::Shell
