#include <karm/entry>

import Karm.App;
import Karm.Core;
import Karm.Ipc;
import Karm.Logger;
import Karm.Sys.Skift;
import Karm.Sys;

import Hjert.Api;
import Strata.Device;

using namespace Karm;
using namespace Karm::Ref::Literals;

namespace Strata::Device {

struct DeviceSession : Ipc::Session {
    explicit DeviceSession(Sys::IpcConnection conn)
        : Session(std::move(conn)) {}

    Async::Task<> handleAsync(Ipc::Message&, Async::CancellationToken) override {
        co_return Ok();
    }
};

struct DeviceHandler : Ipc::Handler {
    Async::Task<Rc<Ipc::Session>> acceptSessionAsync(Sys::IpcConnection connection, Ref::Url const&, Async::CancellationToken) override {
        co_return Ok(makeRc<DeviceSession>(std::move(connection)));
    }
};

struct IsaRootBus : Node {
    Res<> init() override {
        auto i18042port = try$(PortIo::open({0x60, 0x8}));
        try$(attach(makeRc<Ps2::I8042>(i18042port)));

        auto cmosPort = try$(PortIo::open({0x70, 0x2}));
        try$(attach(makeRc<Cmos::Cmos>(cmosPort)));

        return Ok();
    }
};

struct RootBus : Node {
    Ipc::Server& _server;

    RootBus(Ipc::Server& server)
        : _server(server) {}

    Res<> init() override {
        try$(attach(makeRc<IsaRootBus>()));
        return Ok();
    }

    Res<> bubble(App::Event& e) override {
        if (auto me = e.is<App::MouseEvent>()) {
            _server.broadcast<App::MouseEvent>(*me);
            e.accept();
        } else if (auto ke = e.is<App::KeyboardEvent>()) {
            _server.broadcast<App::KeyboardEvent>(*ke);
            e.accept();
        }

        return Node::bubble(e);
    }

    Async::Task<> dispatchAsync(Async::CancellationToken ct) {
        auto pipe = co_try$(Sys::Skift::PipeFd::create());
        Vec<Hj::Irq> irqs = {};
        irqs.ensure(16);
        for (usize i = 0; i < 16; i++) {
            auto irq = co_try$(Hj::Irq::create(Hj::ROOT, i));
            co_try$(irq.bind(pipe->_pipe.cap()));
            irqs.pushBack(std::move(irq));
        }

        while (true) {
            Array<u64, 32> incoming;
            auto n = co_trya$(Sys::globalSched().readAsync(pipe, mutBytes(incoming), ct)) / sizeof(u64);
            for (auto irq : sub(incoming, 0, n)) {
                auto e = App::makeEvent<IrqEvent>(irq);
                auto res = event(*e);
                if (not res)
                    logError("an error occurred during irq{} dispatch: {}", irq, res);
                co_try$(irqs[irq].eoi());
            }
        }
    }
};

} // namespace Strata::Device

Async::Task<> entryPointAsync(Sys::Env&, Async::CancellationToken ct) {
    auto handler = makeRc<Strata::Device::DeviceHandler>();
    auto server = co_trya$(Ipc::Server::createAsync("ipc://strata-device"_url, handler));

    logInfo("devices: building device tree...");
    auto root = makeRc<Strata::Device::RootBus>(server);
    co_try$(root->init());

    co_return co_await Async::join(
        server.servAsync(ct),
        root->dispatchAsync(ct)
    );
}
