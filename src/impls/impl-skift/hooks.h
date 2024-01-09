#pragma once

#include <hjert-api/api.h>
#include <karm-sys/context.h>

struct ChannelsHook : public Sys::Service {
    Hj::Channel _in;
    Hj::Channel _out;

    ChannelsHook(Hj::Cap in, Hj::Cap out)
        : _in(in), _out(out) {}
};

inline ChannelsHook &useChannel(Sys::Ctx &ctx = Sys::globalCtx()) {
    return ctx.use<ChannelsHook>();
}
