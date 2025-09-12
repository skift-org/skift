module;

#include <karm-logger/logger.h>
#include <karm-sys/chan.h>
#include <karm-sys/dir.h>
#include <karm-sys/file.h>
#include <karm-sys/lookup.h>
#include <karm-sys/socket.h>

export module Karm.Http:transport;

import Karm.Core;
import Karm.Ref;

import :request;
import :response;

namespace Karm::Http {

export struct Transport {
    virtual ~Transport() = default;

    virtual Async::Task<Rc<Response>> doAsync(Rc<Request> request) = 0;
};

// MARK: Http Transport --------------------------------------------------------

static constexpr usize BUF_SIZE = 4096;

struct ContentBody : Body {
    Buf<u8> _resumes;
    usize _resumesPos = 0;
    Sys::TcpConnection _conn;
    usize _contentLength;

    ContentBody(Bytes resumes, Sys::TcpConnection conn, usize contentLength)
        : _resumes(resumes),
          _conn(std::move(conn)),
          _contentLength(contentLength - resumes.len()) {
    }

    Async::Task<usize> readAsync(MutBytes buf) override {
        if (_resumesPos < _resumes.len()) {
            usize n = min(buf.len(), _resumes.len() - _resumesPos);
            copy(sub(_resumes, _resumesPos, _resumesPos + n), buf);
            _resumesPos += n;
            co_return n;
        }

        if (_contentLength == 0)
            co_return 0;

        usize n = min(buf.len(), _contentLength);
        n = co_trya$(_conn.readAsync(mutSub(buf, 0, n)));
        _contentLength -= n;
        co_return n;
    }
};

struct ChunkedBody : Body {
    Buf<u8> _buf;
    Sys::TcpConnection _conn;

    ChunkedBody(Bytes resumes, Sys::TcpConnection conn)
        : _buf(resumes), _conn(std::move(conn)) {}
};

struct HttpTransport : Transport {
    Async::Task<> _sendRequestAsync(Request& request, Sys::TcpConnection& conn) {
        Io::StringWriter req;
        request.version = Version{1, 1};
        co_try$(request.unparse(req));
        co_trya$(conn.writeAsync(req.bytes()));

        if (auto body = request.body)
            co_trya$(Aio::copyAsync(**body, conn));

        co_return Ok();
    }

    Async::Task<Rc<Response>> _recvResponseAsync(Sys::TcpConnection& conn) {
        Array<u8, BUF_SIZE> buf = {};
        Io::BufReader reader = sub(buf, 0, co_trya$(conn.readAsync(buf)));
        auto response = co_try$(Response::read(reader));

        if (auto contentLength = response.header.contentLength()) {
            response.body = makeRc<ContentBody>(reader.bytes(), std::move(conn), contentLength.unwrap());
        } else if (auto transferEncoding = response.header.tryGet("Transfer-Encoding"s)) {
            logWarn("Transfer-Encoding: {} not supported", transferEncoding);
        } else {
            // NOTE: When there is no content length, and no transfer encoding,
            //       we read until the server closes the socket.
            response.body = makeRc<ContentBody>(reader.bytes(), std::move(conn), Limits<usize>::MAX);
        }

        co_return Ok(makeRc<Response>(std::move(response)));
    }

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        auto& url = request->url;
        if (url.scheme != "http")
            co_return Error::unsupported("unsupported scheme");

        auto ips = co_trya$(Sys::lookupAsync(url.host.str()));
        auto port = url.port.unwrapOr(80);
        Sys::SocketAddr addr{first(ips), (u16)port};
        auto conn = co_try$(Sys::TcpConnection::connect(addr));
        co_trya$(_sendRequestAsync(*request, conn));
        co_return co_trya$(_recvResponseAsync(conn));
    }
};

export Rc<Transport> httpTransport() {
    return makeRc<HttpTransport>();
}

// MARK: Pipe Transport --------------------------------------------------------

struct PipeBody : Body {
    Buf<u8> _resumes;
    usize _resumesPos = 0;
    usize _contentLength;

    PipeBody(Bytes resumes, usize contentLength = Limits<usize>::MAX)
        : _resumes(resumes),
          _contentLength(contentLength - resumes.len()) {
    }

    Async::Task<usize> readAsync(MutBytes buf) override {
        if (_resumesPos < _resumes.len()) {
            usize n = min(buf.len(), _resumes.len() - _resumesPos);
            copy(sub(_resumes, _resumesPos, _resumesPos + n), buf);
            _resumesPos += n;
            co_return n;
        }

        if (_contentLength == 0)
            co_return 0;

        usize n = min(buf.len(), _contentLength);
        n = co_try$(Sys::in().read(mutSub(buf, 0, n)));
        _contentLength -= n;

        co_return n;
    }
};

struct PipeTransport : Transport {
    Async::Task<> _sendRequest(Request& request) {
        request.version = Version{1, 1};
        co_try$(request.unparse(Sys::out()));

        if (auto body = request.body) {
            auto out = Aio::adapt(Sys::out());
            co_trya$(Aio::copyAsync(**body, out));
        }

        co_return Ok();
    }

    Async::Task<Rc<Response>> _recvResponse() {
        Array<u8, BUF_SIZE> buf = {};
        Io::BufReader reader = sub(buf, 0, co_try$(Sys::in().read(buf)));
        auto response = co_try$(Response::read(reader));
        if (auto contentLength = response.header.contentLength()) {
            response.body = makeRc<PipeBody>(reader.bytes(), contentLength.unwrap());
        } else {
            response.body = makeRc<PipeBody>(reader.bytes());
        }

        co_return Ok(makeRc<Response>(std::move(response)));
    }

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        auto& url = request->url;
        if (url.scheme != "pipe")
            co_return Error::unsupported("unsupported scheme");

        co_trya$(_sendRequest(*request));
        co_return co_trya$(_recvResponse());
    }
};

export Rc<Transport> pipeTransport() {
    return makeRc<PipeTransport>();
}

// MARK: Local -----------------------------------------------------------------

export enum struct LocalTransportPolicy {
    FILTER,
    ALLOW_ALL, // Allow all local resources access, this includes file:, bundle:, and fd:.
};

struct LocalTransport : Transport {
    LocalTransportPolicy _policy;
    Vec<String> _allowed = {};

    LocalTransport(LocalTransportPolicy policy)
        : _policy(policy) {}

    LocalTransport(Vec<String> allowed)
        : _policy(LocalTransportPolicy::FILTER), _allowed(allowed) {}

    Res<Pair<Rc<Body>, Ref::Mime>> _load(Ref::Url url) {
        if (url.scheme == "data") {
            auto blob = try$(url.blob);
            auto body = Body::from(blob);
            return Ok(Pair{body, blob->type});
        }

        if (try$(Sys::isFile(url))) {
            auto body = Body::from(try$(Sys::File::open(url)));
            auto mime = Ref::sniffSuffix(url.path.suffix()).unwrapOr("application/octet-stream"_mime);
            return Ok(Pair{body, mime});
        }

        auto dir = try$(Sys::Dir::open(url));
        Io::StringWriter sw;
        Io::Emit e{sw};
        e("<html><body><h1>Index of {}</h1><ul>", url.path);
        for (auto& diren : dir.entries()) {
            if (diren.hidden())
                continue;
            e("<li><a href=\"{}\">{}</a></li>", url.join(diren.name), diren.name);
        }
        e("</ul></body></html>");
        return Ok(Pair{Body::from(sw.take()), "text/html"_mime});
    }

    Async::Task<> _saveAsync(Ref::Url url, Rc<Body> body) {
        auto file = co_try$(Sys::File::create(url));
        co_trya$(Aio::copyAsync(*body, file));
        co_return Ok();
    }

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        if (_policy == LocalTransportPolicy::FILTER) {
            if (not contains(_allowed, request->url.scheme)) {
                co_return Error::permissionDenied("disallowed by policy");
            }
        }

        auto response = makeRc<Response>();
        response->code = OK;

        if (auto it = request->body;
            it and (request->method == PUT or
                    request->method == POST))
            co_trya$(_saveAsync(request->url, *it));

        if (request->method == GET or request->method == POST) {
            auto [body, mime] = co_try$(_load(request->url));
            response->body = body;
            response->header.add("Content-Type", mime.str());
        }

        co_return Ok(response);
    }
};

export Rc<Transport> localTransport(LocalTransportPolicy policy) {
    return makeRc<LocalTransport>(policy);
}

export Rc<Transport> localTransport(Vec<String> allowed) {
    return makeRc<LocalTransport>(std::move(allowed));
}

// MARK: Fallback --------------------------------------------------------------

struct MultiplexTransport : Transport {
    Vec<Rc<Transport>> _transports;

    MultiplexTransport(Vec<Rc<Transport>> transports)
        : _transports(std::move(transports)) {}

    Async::Task<Rc<Response>> doAsync(Rc<Request> request) override {
        for (auto& transport : _transports) {
            auto res = co_await transport->doAsync(request);
            if (res)
                co_return res.unwrap();

            if (res.none() != Error::UNSUPPORTED)
                co_return res.none();
        }

        co_return Error::notFound("no client could handle the request");
    }
};

export Rc<Transport> multiplexTransport(Vec<Rc<Transport>> transports) {
    return makeRc<MultiplexTransport>(std::move(transports));
}

} // namespace Karm::Http
