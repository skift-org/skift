module;

#include <karm-async/task.h>
#include <karm-base/rc.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>

export module Karm.Http:client;

import :transport;

namespace Karm::Http {

export struct Client : public Transport {
    String userAgent = "Karm-Http/" stringify$(__ck_version_value) ""s;
    Rc<Transport> transport;

    Client(Rc<Transport> transport)
        : transport(std::move(transport)) {}

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        request->header.add("User-Agent", userAgent);
        auto resp = co_trya$(transport->doAsync(request));
        logInfo("\"{} {}\" {} {}", request->method, request->url, toUnderlyingType(resp->code), resp->code);
        co_return Ok(resp);
    }

    Async::Task<Rc<Response>> getAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::GET;
        req->url = url;
        req->version = Version{1, 1};
        req->header.add("Host", url.host);

        return doAsync(req);
    }

    Async::Task<Rc<Response>> headAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::HEAD;
        req->url = url;
        req->version = Version{1, 1};
        req->header.add("Host", url.host);

        return doAsync(req);
    }

    Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
        auto req = makeRc<Request>();
        req->method = Method::POST;
        req->url = url;
        req->version = Version{1, 1};
        req->body = body;
        req->header.add("Host", url.host);

        return doAsync(req);
    }
};

// MARK: Clientless ------------------------------------------------------------

export Rc<Client> defaultClient() {
    return makeRc<Client>(
        multiplexTransport({
            httpTransport(),
            localTransport(),
        })
    );
}

export Async::Task<Rc<Response>> getAsync(Mime::Url url) {
    return defaultClient()->getAsync(url);
}

export Async::Task<Rc<Response>> headAsync(Mime::Url url) {
    return defaultClient()->headAsync(url);
}

export Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
    return defaultClient()->postAsync(url, body);
}

export Async::Task<Rc<Response>> doAsync(Rc<Request> request) {
    return defaultClient()->doAsync(request);
}

} // namespace Karm::Http
