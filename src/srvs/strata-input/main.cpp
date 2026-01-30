#include <karm/entry>

import Karm.Logger;
import Strata.Input;
import Strata.Protos;
import Karm.App;

using namespace Karm;

namespace Strata::Input {

struct InputSession : Sys::IpcSession {
    explicit InputSession(Sys::IpcConnection conn)
        : IpcSession(std::move(conn)) {}

    Async::Task<> handleAsync(Sys::IpcMessage&, Async::CancellationToken) override {
        co_return Ok();
    }
};

struct InputHandler : Sys::IpcHandler {
    Async::Task<Rc<Sys::IpcSession>> acceptSessionAsync(Sys::IpcConnection conn, Async::CancellationToken) override {
        co_return Ok(makeRc<InputSession>(std::move(conn)));
    }
};

struct Service {
    Sys::IpcServer _server;
    Sys::IpcClient _client;
    Keymap _keymap;
    Flags<App::KeyMod> _mods = {};

    static Async::Task<Service> createAsync(Async::CancellationToken ct) {
        auto keymap = Keymap::qwerty();
        auto server = co_trya$(Sys::IpcServer::createAsync("ipc://strata-input"_url, makeRc<Strata::Input::InputHandler>()));
        auto client = co_trya$(Sys::IpcClient::connectAsync("ipc://strata-device"_url, ct));
        co_return Ok(Service{std::move(server), std::move(client), std::move(keymap)});
    }

    Res<> _handleKey(App::Key key) {
        auto chr = _keymap.handle(key, _mods).unwrapOr("");
        if (chr) {
            // logDebug("translated {} to {#}", key.name(), chr);
            for (auto r : iterRunes(chr))
                _server.broadcast(App::TypeEvent{r, _mods});
        }

        return Ok();
    }

    Res<> _handleKeyboardEvent(Sys::IpcMessage& message) {
        auto event = try$(message.unpack<App::KeyboardEvent>());

        if (event.type == App::KeyboardEvent::PRESS) {
            _mods.set(event.key.toMod().unwrapOr({}));
        } else if (event.type == App::KeyboardEvent::RELEASE) {
            _mods.unset(event.key.toMod().unwrapOr({}));
        }

        event.mods = _mods;
        _server.broadcast(event);

        if (event.type == App::KeyboardEvent::PRESS or
            event.type == App::KeyboardEvent::REPEATE) {
            try$(_handleKey(event.key));
        }

        return Ok();
    }

    Res<> _handleMouseEvent(Sys::IpcMessage& message) {
        auto event = try$(message.unpack<App::MouseEvent>());
        event.mods = _mods;
        _server.broadcast(event);
        return Ok();
    }

    Async::Task<> dispatchEventAsync(Async::CancellationToken ct) {
        while (true) {
            auto msg = co_trya$(_client.recvAsync(ct));
            if (msg.is<App::KeyboardEvent>())
                (void)_handleKeyboardEvent(msg);
            else if (msg.is<App::MouseEvent>())
                (void)_handleMouseEvent(msg);
            else
                logWarn("unsupported message: {}", msg.header());
        }
    }

    Async::Task<> servAsync(Async::CancellationToken ct) {
        logDebug("service started");
        co_return co_await Async::join(
            dispatchEventAsync(ct),
            _server.servAsync(ct)
        );
    }
};

} // namespace Strata::Input

Async::Task<> entryPointAsync(Sys::Context&, Async::CancellationToken ct) {
    auto server = co_trya$(Strata::Input::Service::createAsync(ct));
    co_return co_await server.servAsync(ct);
}
