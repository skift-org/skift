#include <karm-async/main.h>
#include <karm-io/funcs.h>
#include <karm-logger/logger.h>
#include <karm-sys/file.h>
#include <karm-sys/socket.h>
#include <web-http/http.h>
#include <web-tls/tls.h>

namespace Web::Server {

Str contentType(Url::Path const &path) {
    auto sufix = path.sufix();
    if (sufix == "html")
        return "text/html; charset=UTF-8";
    if (sufix == "css")
        return "text/css; charset=UTF-8";
    if (sufix == "js")
        return "application/javascript; charset=UTF-8";
    if (sufix == "png")
        return "image/png";
    if (sufix == "jpg")
        return "image/jpeg";
    if (sufix == "ico")
        return "image/x-icon";
    if (sufix == "svg")
        return "image/svg+xml";
    if (sufix == "woff")
        return "font/woff";
    if (sufix == "woff2")
        return "font/woff2";
    if (sufix == "ttf")
        return "font/ttf";
    if (sufix == "eot")
        return "application/vnd.ms-fontobject";
    if (sufix == "otf")
        return "font/otf";
    if (sufix == "json")
        return "application/json";
    if (sufix == "txt")
        return "text/plain; charset=UTF-8";
    return "application/octet-stream";
}

Async::Prom<> respondFile(Sys::_Connection &conn, Url::Url const &url, Http::Code code = Http::Code::OK) {
    auto ct = contentType(url.path);
    auto file = co_try$(Sys::File::open(url));
    auto stat = co_try$(file.stat());

    Io::StringWriter header;
    co_try$(Fmt::format(
        header,
        "HTTP/1.1 {} {}\r\n"
        "Connection: close\r\n"
        "Content-Type: {}\r\n"
        "Content-Length: {}\r\n"
        "X-Powered-By: Karm Web\r\n"
        "\r\n",
        (usize)code,
        Http::toStr(code),
        ct,
        stat.size));

    co_try$(conn.write(header.bytes()));
    co_try$(Io::copy(file, conn));
    co_return Ok();
}

Async::Prom<> respond404(Sys::_Connection &conn) {
    auto res = co_await respondFile(conn, "bundle://web-server/public/404.html"_url, Http::Code::NOT_FOUND);
    if (res)
        co_return Ok();

    Io::StringWriter header;
    co_try$(Fmt::format(
        header,
        "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain; charset=UTF-8\r\n"
        "Content-Length: 9\r\n"
        "X-Powered-By: Karm Web\r\n"
        "\r\n"
        "Not Found"));

    co_try$(conn.write(header.bytes()));
    co_return Ok();
}

Async::Prom<> handleRequest(Sys::_Connection &conn, Str request, Sys::SocketAddr addr) {
    Io::SScan scan{request};
    auto req = co_try$(Http::Request::parse(scan));
    auto url = "bundle://web-server/public/"_url / req.path;

    logInfo("{}: {} {}", addr, req.method, req.path);

    Res<> firstRes = co_await respondFile(conn, url, Http::Code::OK);
    if (firstRes) {
        co_return Ok();
    }

    Res<> res = co_await respondFile(conn, url / "index.html", Http::Code::OK);
    if (res) {
        co_return Ok();
    }

    logWarn("{}: {} {}: {}", addr, req.method, url, firstRes);
    co_return co_await respond404(conn);
}

Async::Prom<> handleConnection(Sys::TcpConnection stream) {
    Array<char, 4096> buf;
    auto len = co_try$(stream.read(mutBytes(buf)));

    if (not Tls::isHello(bytes(buf))) {
        co_try_await$(handleRequest(stream, Str{buf.buf(), len}, stream.addr()));
        co_return Ok();
    }

    logDebug("{}: wants TLS", stream.addr());
    auto tls = co_try$(Tls::TlsConnection::accept(stream, bytes(buf)));
    len = co_try$(tls.read(mutBytes(buf)));
    co_try_await$(handleRequest(tls, Str{buf.buf(), len}, stream.addr()));
    co_return Ok();
}

} // namespace Web::Server

Async::Prom<> entryPointAsync(Ctx &) {
    auto listener = co_try$(Sys::TcpListener::listen(Sys::Ip4::localhost(8080)));
    logInfo("Serving on http://{}", listener.addr());

    while (true) {
        auto stream = co_try$(listener.accept());
        auto res = co_await Web::Server::handleConnection(std::move(stream));
        if (not res)
            logError("Connection error: {}", res);
    }
}
