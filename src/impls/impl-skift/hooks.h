#pragma once

#include <hjert-api/api.h>
#include <karm-main/base.h>

struct ChannelsHook : public Hook {
    Hj::Channel _in;
    Hj::Channel _out;

    ChannelsHook(Hj::Cap in, Hj::Cap out)
        : _in(in), _out(out) {}
};

inline ChannelsHook &useChannel(Ctx &ctx = Ctx::instance()) {
    return ctx.use<ChannelsHook>();
}
