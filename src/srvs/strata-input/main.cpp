#include <karm/entry>

import Karm.Logger;
import Karm.App;
import Karm.Ipc;
import Strata.Input;
import Strata.Protos;

using namespace Karm;
using namespace Karm::Ref::Literals;

namespace Strata::Input {

struct InputSession : Ipc::Session {
    explicit InputSession(Sys::IpcConnection conn)
        : Session(std::move(conn)) {}

    Async::Task<> handleAsync(Ipc::Message&, Async::CancellationToken) override {
        co_return Ok();
    }
};

struct InputHandler : Ipc::Handler {
    Async::Task<Rc<Ipc::Session>> acceptSessionAsync(Sys::IpcConnection conn, Async::CancellationToken) override {
        co_return Ok(makeRc<InputSession>(std::move(conn)));
    }
};

struct Service {
    Ipc::Server _server;
    Ipc::Client _client;
    Keymap _keymap;
    Flags<App::KeyMod> _mods = {};

    static Async::Task<Service> createAsync(Async::CancellationToken ct) {
        auto keymap = Keymap::qwerty();
        auto server = co_trya$(Ipc::Server::createAsync("ipc://strata-input"_url, makeRc<Strata::Input::InputHandler>()));
        auto client = co_trya$(Ipc::Client::connectAsync("ipc://strata-device"_url, ct));
        co_return Ok(Service{std::move(server), std::move(client), std::move(keymap)});
    }

    Res<> _handleKeyboardEvent(Ipc::Message& message) {
        auto event = try$(message.unpack<App::KeyboardEvent>());

        if (event.type == App::KeyboardEvent::PRESS) {
            _mods.set(event.key.toMod().unwrapOr({}));
        } else if (event.type == App::KeyboardEvent::RELEASE) {
            _mods.unset(event.key.toMod().unwrapOr({}));
        }

        event.mods = _mods;

        auto chr = _keymap.handle(event.key, _mods).unwrapOr("");
        if (chr) {
            for (auto r : iterRunes(chr)) {
                event.rune = r;
                _server.broadcast(event);
            }
        } else {
            _server.broadcast(event);
        }

        return Ok();
    }

    Res<> _handleMouseEvent(Ipc::Message& message) {
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

Async::Task<> entryPointAsync(Sys::Env&, Async::CancellationToken ct) {
    auto server = co_trya$(Strata::Input::Service::createAsync(ct));
    co_return co_await server.servAsync(ct);
}
