#include <grund-bus/api.h>
#include <grund-shell/api.h>
#include <karm-ui/_embed.h>

namespace Karm::Ui::_Embed {

Res<Rc<Host>> makeHost(Child) {
    notImplemented();
}

struct Host : public Ui::ProxyNode<Host> {
    Rpc::Endpoint &_endpoint;
    Rpc::Port _shell;
    Grund::Shell::Api::Instance _instance;

    Host(Child child, Rpc::Endpoint &endpoint, Rpc::Port shell, Grund::Shell::Api::Instance surface)
        : Ui::ProxyNode<Host>(std::move(child)), _endpoint(endpoint), _shell(shell), _instance(surface) {}
};

Async::Task<> runAsync(Sys::Context &ctx, Child root) {
    auto endpoint = Rpc::Endpoint::create(ctx);
    auto shell = co_trya$(endpoint.callAsync<Grund::Bus::Api::Locate>(Rpc::Port::BUS, "grund-shell"s));
    auto size = root->size({1024, 720}, Hint::MIN);
    auto surface = co_trya$(endpoint.callAsync<Grund::Shell::Api::CreateInstance>(shell, size));

    auto host = makeRc<Host>(std::move(root), endpoint, shell, surface);

    while (true) {
        auto msg = co_trya$(endpoint.recvAsync());
        logWarn("unsupported event: {}", msg.header());
    }
}

} // namespace Karm::Ui::_Embed
