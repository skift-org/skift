#include <karm/entry>

import Karm.Core;
import Karm.App;
import Karm.Sys;
import Karm.Sys.Skift;
import Strata.Device;
import Karm.Logger;
import Hjert.Api;

using namespace Karm;

namespace Strata::Device {

struct DeviceSession : Sys::IpcSession {
    explicit DeviceSession(Sys::IpcConnection conn)
        : Sys::IpcSession(std::move(conn)) {}

    Async::Task<> handleAsync(Sys::IpcMessage&, Async::CancellationToken) override {
        co_return Ok();
    }
};

struct DeviceHandler : Sys::IpcHandler {
    Async::Task<Rc<Sys::IpcSession>> acceptSessionAsync(Sys::IpcConnection conn, Async::CancellationToken) override {
        co_return Ok(makeRc<DeviceSession>(std::move(conn)));
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
    Sys::IpcServer& _server;

    RootBus(Sys::IpcServer& server)
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

Async::Task<> entryPointAsync(Sys::Context&, Async::CancellationToken ct) {
    auto handler = makeRc<Strata::Device::DeviceHandler>();
    auto server = co_trya$(Sys::IpcServer::createAsync("ipc://strata-device"_url, handler));

    logInfo("devices: building device tree...");
    auto root = makeRc<Strata::Device::RootBus>(server);
    co_try$(root->init());

    co_return co_await Async::join(
        server.servAsync(ct),
        root->dispatchAsync(ct)
    );
}
