#pragma once

#include <karm-main/main.h>

#include "async.h"

Async::Task<Res<>> entryPointAsync(Ctx &);

Res<> entryPoint(Ctx &ctx) {
    auto task = entryPointAsync(ctx);
    struct Sink : public Async::Sink {
        Res<> post(Async::Event &e) override {
            if (auto *ret = e.is<Async::Task<Res<>>::Ret>())
                Async::loop().quit(ret->res);
            return Ok();
        }
    };
    Sink sink;
    task.bind(sink);
    return Async::loop().run();
}
