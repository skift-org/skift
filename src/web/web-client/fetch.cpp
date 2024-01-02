#include <karm-io/funcs.h>
#include <karm-logger/logger.h>
#include <karm-sys/file.h>
#include <karm-sys/socket.h>
#include <web-dns/dns.h>
#include <web-http/http.h>
#include <web-tls/tls.h>

#include "fetch.h"

namespace Web::Client {

Async::Prom<Sys::Ip> resolve(Str host) {
    auto res = Sys::Ip::parse(host);
    if (res)
        co_return res;

    if (host == "localhost")
        co_return Ok(Sys::Ip4::localhost());

    auto dns = co_try$(Dns::Client::connect(Dns::GOOGLE));
    co_return Ok(co_try$(dns.resolve(host)));
}

Async::Prom<usize> _fetch(Url::Url const &url, Sys::_Connection &conn, Io::Writer &out) {
    // Send request
    logDebug("GET {} HTTP/1.1", url.path);
    Io::StringWriter req;
    co_try$(Fmt::format(
        req,
        "GET {} HTTP/1.1\r\n"
        "Host: {}\r\n"
        "Connection: close\r\n"
        "User-Agent: Karm Web/" stringify$(__ck_version_value) "\r\n"
                                                               "\r\n",
        url.path,
        url.host));
    co_try$(conn.write(req.bytes()));

    // Read response
    Array<char, 4096> buf;
    auto len = co_try$(conn.read(mutBytes(buf)));
    logDebug("Response: {}", Str{buf.buf(), len});

    Str respStr{buf.buf(), len};

    auto s = Io::SScan({buf.buf(), len});
    auto resp = co_try$(Http::Response::parse(s));

    if (resp.code != Http::Code::OK)
        co_return Error::invalidData("http error");

    auto contentLength = co_try$(Io::parseUint(co_try$(resp.headers.get("Content-Length"))));
    auto remData = bytes(s.remStr());
    auto written = co_try$(out.write(sub(remData, 0, contentLength)));
    if (written < contentLength)
        co_try$(Io::copy(conn, out, contentLength - written));
    co_return Ok(contentLength);
}

Async::Prom<usize> fetch(Url::Url const &url, Io::Writer &out) {
    auto ip = co_try_await$(resolve(url.host));
    auto port = url.port ? *url.port : 80;
    if (port > 65535)
        co_return Error::invalidData("port out of range");

    Sys::SocketAddr addr{ip, (u16)port};

    if (url.scheme == "http") {
        auto conn = co_try$(Sys::TcpConnection::connect(addr));
        co_return co_await _fetch(url, conn, out);
    } else if (url.scheme == "https") {
        auto conn = co_try$(Sys::TcpConnection::connect(addr));
        auto tls = co_try$(Web::Tls::TlsConnection::connect(conn));
        co_return co_await _fetch(url, tls, out);
    } else {
        co_return Error::invalidData("unsupported scheme");
    }
}

Async::Prom<String> fetchString(Url::Url const &url) {
    Io::StringWriter out;
    co_try_await$(fetch(url, out));
    co_return Ok(out.take());
}

Async::Prom<Json::Value> fetchJson(Url::Url const &url) {
    auto str = co_try_await$(fetchString(url));
    co_return Ok(co_try$(Json::parse(str)));
}

} // namespace Web::Client
