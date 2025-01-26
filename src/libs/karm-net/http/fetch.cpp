#include <karm-io/funcs.h>
#include <karm-json/parse.h>
#include <karm-logger/logger.h>
#include <karm-net/dns/dns.h>
#include <karm-net/http/http.h>
#include <karm-net/tls/tls.h>
#include <karm-sys/file.h>
#include <karm-sys/socket.h>

#include "fetch.h"

namespace Karm::Net::Http {

Async::Task<Sys::Ip> resolve(Str host) {
    auto res = Sys::Ip::parse(host);
    if (res)
        co_return res;

    if (host == "localhost")
        co_return Ok(Sys::Ip4::localhost());

    auto dns = co_try$(Dns::Client::connect(Dns::GOOGLE));
    co_return Ok(co_try$(dns.resolve(host)));
}

Async::Task<usize> _fetch(Mime::Url const& url, Sys::_Connection& conn, Io::Writer& out) {
    // Send request
    logDebug("GET {} HTTP/1.1", url.path);
    Io::StringWriter req;
    co_try$(Io::format(
        req,
        "GET {} HTTP/1.1\r\n"
        "Host: {}\r\n"
        "Connection: close\r\n"
        "User-Agent: Karm Web Fetch/" stringify$(__ck_version_value) "\r\n"
                                                                     "\r\n",
        url.path,
        url.host
    ));

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

    auto contentLength = co_try$(Io::atou(co_try$(resp.headers.tryGet("Content-Length"))));
    auto remData = bytes(s.remStr());
    auto written = co_try$(out.write(sub(remData, 0, contentLength)));
    if (written < contentLength)
        co_try$(Io::copy(conn, out, contentLength - written));
    co_return Ok(contentLength);
}

Async::Task<usize> fetch(Mime::Url const& url, Io::Writer& out) {
    auto ip = co_trya$(resolve(url.host));
    auto port = url.port ? *url.port : 80;
    if (port > 65535)
        co_return Error::invalidData("port out of range");

    Sys::SocketAddr addr{ip, (u16)port};

    if (url.scheme == "http") {
        auto conn = co_try$(Sys::TcpConnection::connect(addr));
        co_return co_await _fetch(url, conn, out);
    } else if (url.scheme == "https") {
        auto conn = co_try$(Sys::TcpConnection::connect(addr));
        auto tls = co_try$(Tls::TlsConnection::connect(conn));
        co_return co_await _fetch(url, tls, out);
    } else {
        co_return Error::invalidData("unsupported scheme");
    }
}

Async::Task<String> fetchString(Mime::Url const& url) {
    Io::StringWriter out;
    co_trya$(fetch(url, out));
    co_return Ok(out.take());
}

Async::Task<Json::Value> fetchJson(Mime::Url const& url) {
    auto str = co_trya$(fetchString(url));
    co_return Ok(co_try$(Json::parse(str)));
}

} // namespace Karm::Net::Http
