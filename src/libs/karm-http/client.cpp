module;

#include <karm-async/task.h>
#include <karm-base/rc.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-sys/lookup.h>
#include <karm-sys/socket.h>

export module Karm.Http:client;

import Karm.Aio;
import :request;
import :response;

namespace Karm::Http {

export struct Client {
    static Rc<Client> simple() {
        notImplemented();
    }

    virtual ~Client() = default;

    virtual Async::Task<Rc<Response>> doAsync(Rc<Request> request) = 0;

    Async::Task<Rc<Response>> getAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::GET;
        req->url = url;
        req->version = Version{1, 1};
        return doAsync(req);
    }

    Async::Task<Rc<Response>> headAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::HEAD;
        req->url = url;
        req->version = Version{1, 1};
        return doAsync(req);
    }

    Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
        auto req = makeRc<Request>();
        req->method = Method::POST;
        req->url = url;
        req->version = Version{1, 1};
        req->body = body;
        return doAsync(req);
    }
};

// MARK: Clientless ------------------------------------------------------------

export Async::Task<Rc<Response>> getAsync(Mime::Url url) {
    return Client::simple()->getAsync(url);
}

export Async::Task<Rc<Response>> headAsync(Mime::Url url) {
    return Client::simple()->headAsync(url);
}

export Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
    return Client::simple()->postAsync(url, body);
}

export Async::Task<Rc<Response>> doAsync(Rc<Request> request) {
    return Client::simple()->doAsync(request);
}

} // namespace Karm::Http
