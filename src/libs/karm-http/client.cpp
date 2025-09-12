module;

#include <karm-core/macros.h>
#include <karm-logger/logger.h>

export module Karm.Http:client;

import Karm.Core;
import Karm.Debug;
import Karm.Ref;

import :transport;

namespace Karm::Http {

static Debug::Flag debugClient = "http-client"s;

export struct Client : Transport {
    String userAgent = "Karm-Http/" stringify$(__ck_version_value) ""s;
    Rc<Transport> _transport;

    Client(Rc<Transport> transport)
        : _transport(std::move(transport)) {}

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        request->header.add("User-Agent", userAgent);
        auto maybeResp = co_await _transport->doAsync(request);
        if (not maybeResp) {
            logDebugIf(debugClient, "\"{} {}\" {}", request->method, request->url, maybeResp.none());
            co_return maybeResp.none();
        }

        auto resp = maybeResp.unwrap();
        logDebugIf(debugClient, "\"{} {}\" {} {}", request->method, request->url, toUnderlyingType(resp->code), resp->code);
        co_return Ok(resp);
    }

    [[clang::coro_wrapper]]
    Async::Task<Rc<Response>> getAsync(Ref::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::GET;
        req->url = url;
        req->header.add("Host", url.host.str());

        return doAsync(req);
    }

    [[clang::coro_wrapper]]
    Async::Task<Rc<Response>> headAsync(Ref::Url url) {
        auto req = makeRc<Request>();
        req->method = Method::HEAD;
        req->url = url;
        req->header.add("Host", url.host.str());

        return doAsync(req);
    }

    [[clang::coro_wrapper]]
    Async::Task<Rc<Response>> postAsync(Ref::Url url, Rc<Body> body) {
        auto req = makeRc<Request>();
        req->method = Method::POST;
        req->url = url;
        req->body = body;
        req->header.add("Host", url.host.str());

        return doAsync(req);
    }
};

// MARK: Clientless ------------------------------------------------------------

export Rc<Client> defaultClient() {
    return makeRc<Client>(
        multiplexTransport({
            httpTransport(),
            localTransport(LocalTransportPolicy::ALLOW_ALL),
        })
    );
}

export Async::Task<Rc<Response>> getAsync(Ref::Url url) {
    auto client = defaultClient();
    co_return co_await client->getAsync(url);
}

export Async::Task<Rc<Response>> headAsync(Ref::Url url) {
    auto client = defaultClient();
    co_return co_await client->headAsync(url);
}

export Async::Task<Rc<Response>> postAsync(Ref::Url url, Rc<Body> body) {
    auto client = defaultClient();
    co_return co_await client->postAsync(url, body);
}

export Async::Task<Rc<Response>> doAsync(Rc<Request> request) {
    auto client = defaultClient();
    co_return co_await client->doAsync(request);
}

} // namespace Karm::Http
