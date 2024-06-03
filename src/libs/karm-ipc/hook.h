#pragma once

#include <karm-sys/context.h>
#include <karm-sys/socket.h>

struct ChannelHook : public Sys::Service {
    Sys::IpcConnection con;

    ChannelHook(Sys::IpcConnection con)
        : con(std::move(con)) {}
};

inline ChannelHook &useChannel(Sys::Ctx &ctx = Sys::globalCtx()) {
    return ctx.use<ChannelHook>();
}
