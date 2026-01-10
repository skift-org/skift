#include <karm-sys/entry.h>

import Karm.Logger;
import Strata.Input;
import Strata.Protos;
import Karm.App;

using namespace Karm;

namespace Strata::Input {

struct Server {
    Sys::Endpoint& _endpoint;
    Keymap _keymap;
    Flags<App::KeyMod> _mods = {};

    static Res<Server> create(Sys::Context&, Sys::Endpoint& endpoint) {
        auto keymap = Keymap::qwerty();
        try$(endpoint.send(
            Sys::Port::BUS,
            Strata::IBus::Listen{
                Meta::idOf<App::KeyboardEvent>()
            }
        ));
        return Ok(Server{endpoint, std::move(keymap)});
    }

    Res<> _handleKey(App::Key key) {
        auto chr = _keymap.handle(key, _mods).unwrapOr("");
        if (chr) {
            logDebug("translated {} to {#}", key.name(), chr);
            for (auto r : iterRunes(chr))
                try$(_endpoint.send<App::TypeEvent>(Sys::Port::BROADCAST, {r, _mods}));
        }
        return Ok();
    }

    Res<> _handleKeyboardEvent(Sys::Message& message) {
        auto event = try$(message.unpack<App::KeyboardEvent>());
        if (event.type == App::KeyboardEvent::PRESS) {
            _mods.set(event.key.toMod().unwrapOr({}));
            try$(_handleKey(event.key));
        } else if (event.type == App::KeyboardEvent::REPEATE) {
            try$(_handleKey(event.key));
        } else if (event.type == App::KeyboardEvent::RELEASE) {
            _mods.unset(event.key.toMod().unwrapOr({}));
        }
        return Ok();
    }

    Async::Task<> servAsync(Async::CancellationToken) {
        logInfo("service started");
        while (true) {
            auto msg = co_trya$(_endpoint.recvAsync());
            if (msg.is<App::KeyboardEvent>())
                (void)_handleKeyboardEvent(msg);
            else
                logWarn("unsupported message: {}", msg.header());
        }
    }
};

} // namespace Strata::Input

Async::Task<> entryPointAsync(Sys::Context& ctx, Async::CancellationToken ct) {
    auto endpoint = Sys::Endpoint::adopt(ctx);
    auto server = co_try$(Strata::Input::Server::create(ctx, endpoint));
    co_return co_await server.servAsync(ct);
}
