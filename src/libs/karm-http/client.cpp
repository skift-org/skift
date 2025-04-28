module;

#include <karm-async/task.h>
#include <karm-base/rc.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>

export module Karm.Http:client;

import :transport;

namespace Karm::Http {

static constexpr bool DEBUG_CLIENT = false;

export struct Client : Transport {
    String userAgent = "Karm-Http/" stringify$(__ck_version_value) ""s;
    Rc<Transport> _transport;

    Client(Rc<Transport> transport)
        : _transport(std::move(transport)) {}

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        request->header.add("User-Agent", userAgent);
        auto maybeResp = co_await _transport->doAsync(request);
        if (not maybeResp) {
            logDebugIf(DEBUG_CLIENT, "\"{} {}\" {}", request->method, request->url, maybeResp.none());
            co_return maybeResp.none();
        }

        auto resp = maybeResp.unwrap();
        logDebugIf(DEBUG_CLIENT, "\"{} {}\" {} {}", request->method, request->url, toUnderlyingType(resp->code), resp->code);
        co_return Ok(resp);
    }

    [[clang::coro_wrapper]]
    Async::Task<Rc<Response>> getAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::GET;
        req->url = url;
        req->header.add("Host", url.host);

        return doAsync(req);
    }

    [[clang::coro_wrapper]]
    Async::Task<Rc<Response>> headAsync(Mime::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::HEAD;
        req->url = url;
        req->header.add("Host", url.host);

        return doAsync(req);
    }

    [[clang::coro_wrapper]]
    Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
        auto req = makeRc<Request>();
        req->method = Method::POST;
        req->url = url;
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
    auto client = defaultClient();
    co_return co_await client->getAsync(url);
}

export Async::Task<Rc<Response>> headAsync(Mime::Url url) {
    auto client = defaultClient();
    co_return co_await client->headAsync(url);
}

export Async::Task<Rc<Response>> postAsync(Mime::Url url, Rc<Body> body) {
    auto client = defaultClient();
    co_return co_await client->postAsync(url, body);
}

export Async::Task<Rc<Response>> doAsync(Rc<Request> request) {
    auto client = defaultClient();
    co_return co_await client->doAsync(request);
}

} // namespace Karm::Http
