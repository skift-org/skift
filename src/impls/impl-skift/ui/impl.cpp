module;

#include <karm-core/macros.h>
#include <karm-gfx/cpu/canvas.h>
#include <karm-logger/logger.h>

module Karm.Ui;

import Strata.Protos;
import Karm.Sys;

namespace Karm::Ui::_Embed {

struct Host : ProxyNode<Host> {
    Sys::Endpoint& _endpoint;
    Sys::Port _shell;
    Strata::IShell::WindowId _window;

    Host(Child child, Sys::Endpoint& endpoint, Sys::Port shell, Strata::IShell::WindowId window)
        : Ui::ProxyNode<Host>(std::move(child)), _endpoint(endpoint), _shell(shell), _window(window) {}
};

Async::Task<> runAsync(Sys::Context& ctx, Child root) {
    auto endpoint = Sys::Endpoint::adopt(ctx);
    auto shell = co_trya$(endpoint.callAsync<Strata::IBus::Locate>(Sys::Port::BUS, "strata-shell"s));
    auto size = root->size({1024, 720}, Hint::MIN);

    auto [window, actual] = co_trya$(endpoint.callAsync<Strata::IShell::WindowCreate>(shell, size));

    auto surface = co_try$(Strata::Protos::Surface::create(actual.size));
    root->layout(actual.size);

    Gfx::CpuCanvas g;
    g.begin(surface->mutPixels());
    g.clear(Ui::GRAY950);
    root->paint(g, actual.size);
    g.end();

    co_try$(endpoint.send<Strata::IShell::WindowAttach>(shell, window, surface));
    logDebug("window atteched");

    auto host = makeRc<Host>(std::move(root), endpoint, shell, window);

    while (true) {
        auto msg = co_trya$(endpoint.recvAsync());
        logWarn("unsupported event: {}", msg.header());
    }
}

} // namespace Karm::Ui::_Embed
