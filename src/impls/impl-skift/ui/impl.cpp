module;

#include <strata-bus/api.h>
#include <strata-shell/api.h>
#include <karm-sys/endpoint.h>
#include <karm-logger/logger.h>

module Karm.Ui:_embed;

import Karm.Ui;

namespace Karm::Ui::_Embed {

struct Host : public Ui::ProxyNode<Host> {
    Sys::Endpoint& _endpoint;
    Sys::Port _shell;
    Strata::Shell::Api::Instance _instance;

    Host(Child child, Sys::Endpoint& endpoint, Sys::Port shell, Strata::Shell::Api::Instance surface)
        : Ui::ProxyNode<Host>(std::move(child)), _endpoint(endpoint), _shell(shell), _instance(surface) {}
};

Async::Task<> runAsync(Sys::Context& ctx, Child root) {
    auto endpoint = Sys::Endpoint::create(ctx);
    auto shell = co_trya$(endpoint.callAsync<Strata::Bus::Api::Locate>(Sys::Port::BUS, "strata-shell"s));
    auto size = root->size({1024, 720}, Hint::MIN);
    auto surface = co_trya$(endpoint.callAsync<Strata::Shell::Api::CreateInstance>(shell, size));

    auto host = makeRc<Host>(std::move(root), endpoint, shell, surface);

    while (true) {
        auto msg = co_trya$(endpoint.recvAsync());
        logWarn("unsupported event: {}", msg.header());
    }
}

} // namespace Karm::Ui::_Embed
