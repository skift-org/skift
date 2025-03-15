module;

#include <karm-async/task.h>
#include <karm-base/rc.h>

export module Karm.Http:server;

import :request;
import :response;

namespace Karm::Http {

export struct Service {
    virtual ~Service() = default;
    virtual Async::Task<> handleAsync(Rc<Request>, Rc<Response::Writer>) = 0;
};

export struct Server {
    static Rc<Server> simple(Rc<Service> srv);

    Rc<Service> _srv;
    virtual ~Server() = default;
    virtual Async::Task<> serveAsync() = 0;
};

// MARK: Serverless ------------------------------------------------------------

export Async::Task<> servAsync(Rc<Service> srv) {
    return Server::simple(srv)->serveAsync();
}

} // namespace Karm::Http
