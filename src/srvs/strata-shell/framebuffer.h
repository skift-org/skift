#pragma once

import Karm.Gfx;

#include <handover/hook.h>
#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-math/rect.h>

namespace Strata::Shell {

struct Framebuffer {
    static Res<Rc<Framebuffer>> open(Sys::Context& ctx);

    Handover::Record _record;
    Hj::Mapped _map;

    Framebuffer(Handover::Record record, Hj::Mapped map);

    Gfx::MutPixels mutPixels();

    Math::Recti bound() const {
        return {
            _record.fb.width,
            _record.fb.height,
        };
    }
};

} // namespace Strata::Shell
