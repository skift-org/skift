#include <karm-io/funcs.h>
#include <karm-logger/logger.h>
#include <karm-mime/mime.h>
#include <karm-sys/entry-async.h>
#include <karm-sys/file.h>
#include <karm-sys/socket.h>
#include <web-http/http.h>
#include <web-tls/tls.h>

namespace Web::Server {

Async::Task<> respondFile(Sys::_Connection &conn, Mime::Url const &url, Http::Code code = Http::Code::OK) {
    auto ct = tryOr(Mime::sniffSuffix(url.path.suffix()), "application/octet-stream"_mime);
    auto file = co_try$(Sys::File::open(url));
    auto stat = co_try$(file.stat());

    Io::StringWriter header;
    co_try$(Io::format(
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
        stat.size
    ));

    co_try$(conn.write(header.bytes()));
    co_try$(Io::copy(file, conn));
    co_return Ok();
}

Async::Task<> respond404(Sys::_Connection &conn) {
    auto res = co_await respondFile(conn, "bundle://web-http-serv/public/404.html"_url, Http::Code::NOT_FOUND);
    if (res)
        co_return Ok();

    Io::StringWriter header;
    co_try$(Io::format(
        header,
        "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain; charset=UTF-8\r\n"
        "Content-Length: 9\r\n"
        "X-Powered-By: Karm Web\r\n"
        "\r\n"
        "Not Found"
    ));

    co_trya$(conn.writeAsync(header.bytes()));
    co_return Ok();
}

Async::Task<> handleRequest(Sys::_Connection &conn, Str request, Sys::SocketAddr addr) {
    Io::SScan scan{request};
    auto req = co_try$(Http::Request::parse(scan));
    auto url = "bundle://web-http-serv/public/"_url / req.path;

    logInfo("{}: {} {}", addr, req.method, req.path);

    Res<> firstRes = co_await respondFile(conn, url, Http::Code::OK);
    if (firstRes) {
        co_return Ok();
    }

    Res<> res = co_await respondFile(conn, url / "index.html", Http::Code::OK);
    if (res)
        co_return Ok();

    logWarn("{}: {} {}: {}", addr, req.method, url, firstRes);
    co_return co_await respond404(conn);
}

Async::Task<> handleConnection(Sys::TcpConnection stream) {
    Array<char, 4096> buf;
    auto len = co_trya$(stream.readAsync(mutBytes(buf)));
    if (not Tls::isHello(bytes(buf))) {
        co_return co_await handleRequest(stream, Str{buf.buf(), len}, stream.addr());
    } else {
        logDebug("{}: wants TLS", stream.addr());
        auto tls = co_try$(Tls::TlsConnection::accept(stream, bytes(buf)));
        len = co_trya$(tls.readAsync(mutBytes(buf)));
        co_return co_await handleRequest(tls, Str{buf.buf(), len}, stream.addr());
    }
}

} // namespace Web::Server

Async::Task<> entryPointAsync(Sys::Ctx &) {
    auto listener = co_try$(Sys::TcpListener::listen(Sys::Ip4::localhost(8080)));
    logInfo("Serving on http://{}", listener.addr());
    while (true)
        Async::detach(Web::Server::handleConnection(co_trya$(listener.acceptAsync())));
}
