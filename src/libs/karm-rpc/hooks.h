#pragma once

#include <karm-sys/context.h>
#include <karm-sys/socket.h>

struct ChannelHook : Sys::Service {
    Sys::IpcConnection con;

    ChannelHook(Sys::IpcConnection con)
        : con(std::move(con)) {}
};

inline ChannelHook& useChannel(Sys::Context& ctx) {
    return ctx.use<ChannelHook>();
}
