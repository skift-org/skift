export module Karm.Http:server;

import Karm.Core;
import :request;
import :response;

namespace Karm::Http {

export struct Service {
    virtual ~Service() = default;
    virtual Async::Task<> handleAsync(Rc<Request>, Rc<Response::Writer>) = 0;
};

export struct Server {
    static Rc<Server> simple(Rc<Service>) {
        notImplemented();
    }

    Rc<Service> _srv;
    virtual ~Server() = default;
    virtual Async::Task<> serveAsync() = 0;
};

// MARK: Serverless ------------------------------------------------------------

export Async::Task<> servAsync(Rc<Service> srv) {
    auto server = Server::simple(srv);
    co_return co_await server->serveAsync();
}

} // namespace Karm::Http
