#pragma once

#include <karm-sys/context.h>
#include <karm-sys/proc.h>
#include <karm-ui/host.h>

namespace Grund::Shell {

struct RootHost :
    public Ui::Host {
    Gfx::MutPixels _front;
    Strong<Gfx::Surface> _back;

    RootHost(Ui::Child root, Gfx::MutPixels front, Strong<Gfx::Surface> back)
        : Ui::Host(root),
          _front(front),
          _back(back) {
    }

    Gfx::MutPixels mutPixels() override {
        return _back->mutPixels();
    }

    void flip(Slice<Math::Recti> dirty) override {
        for (auto d : dirty) {
            Gfx::blitUnsafe(_front.clip(d), _back->pixels().clip(d));
        }
    }

    Res<> wait(TimeStamp until) override {
        try$(Sys::sleepUntil(until));
        return Ok();
    }

    void bubble(App::Event &e) override {
        Ui::Host::bubble(e);
    }
};

Res<Strong<RootHost>> makeHost(Sys::Context &ctx, Ui::Child root);

} // namespace Grund::Shell
