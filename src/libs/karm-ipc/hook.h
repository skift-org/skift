#pragma once

#include <karm-sys/context.h>
#include <karm-sys/socket.h>

struct ChannelHook : public Sys::Service {
    Sys::IpcConnection con;

    ChannelHook(Sys::IpcConnection con)
        : con(std::move(con)) {}
};

inline ChannelHook &useChannel(Sys::Context &ctx = Sys::globalContext()) {
    return ctx.use<ChannelHook>();
}
