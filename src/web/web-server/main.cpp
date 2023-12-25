#include <karm-async/main.h>
#include <karm-io/funcs.h>
#include <karm-logger/logger.h>
#include <karm-sys/file.h>
#include <karm-sys/socket.h>
#include <web-http/http.h>

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

Async::Prom<> respondFile(Sys::TcpConnection &stream, Url::Url const &url, Web::Http::Code code) {
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
        Web::Http::toStr(code),
        ct,
        stat.size));

    co_try$(stream.write(header.bytes()));
    co_try$(Io::copy(file, stream));

    co_return Ok();
}

Async::Prom<> handleConnection(Sys::TcpConnection stream) {
    Array<char, 4096> buf;
    auto len = co_try$(stream.read(mutBytes(buf)));

    Str reqStr{buf.buf(), len};
    Io::SScan reqScan{reqStr};
    auto req = co_try$(Web::Http::Request::parse(reqScan));
    auto url = "bundle://web-server/public/"_url / req.path;

    logInfo("{}: {} {}", stream._addr, req.method, url);

    Res<> firstRes = co_await respondFile(stream, url, Web::Http::Code::OK);
    if (firstRes)
        co_return Ok();

    Res<> res = co_await respondFile(stream, url / "index.html", Web::Http::Code::OK);
    if (res)
        co_return Ok();

    logWarn("{}: {} {}: {}", stream._addr, req.method, url, firstRes);
    res = co_await respondFile(stream, url / "404.html", Web::Http::Code::NOT_FOUND);
    if (res)
        co_return Ok();

    res = co_await respondFile(stream, "bundle://web-server/public/404.html"_url, Web::Http::Code::NOT_FOUND);
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

    co_try$(stream.write(header.bytes()));
    co_return Ok();
}

Async::Prom<> entryPointAsync(Ctx &) {
    auto listener = co_try$(Sys::TcpListener::listen(Sys::Ip4::localhost(8080)));
    logInfo("Serving on http://{}", listener._addr);

    while (true) {
        auto stream = co_try$(listener.accept());
        auto res = co_await handleConnection(std::move(stream));
        if (not res)
            logError("Connection error: {}", res);
    }
}
