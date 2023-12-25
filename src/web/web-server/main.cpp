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

Async::Prom<> entryPointAsync(Ctx &) {
    auto listener = co_try$(Sys::TcpListener::listen(Sys::Ip4::localhost(8080)));
    logInfo("Serving on http://{}", listener._addr);

    while (true) {
        auto stream = co_try$(listener.accept());

        Array<char, 4096> buf;
        auto len = co_try$(stream.read(mutBytes(buf)));

        Str reqStr{buf.buf(), len};
        Io::SScan reqScan{reqStr};
        auto req = co_try$(Web::Http::Request::parse(reqScan));
        auto fileUrl = "bundle://web-server/public/"_url / req.path;
        auto ct = contentType(req.path);
        auto maybeFile = Sys::File::open(fileUrl);
        Io::StringWriter header;

        if (not maybeFile.has()) {
            logWarn("{}: {} {} -> 404", stream._addr, req.method, req.path);
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
            continue;
        }

        logInfo("{}: {} {} -> {} ({})", stream._addr, req.method, req.path, fileUrl, ct);
        auto &file = maybeFile.unwrap();
        co_try$(Fmt::format(
            header,
            "HTTP/1.1 200 OK\r\n"
            "Connection: close\r\n"
            "Content-Type: {}\r\n"
            "Content-Length: {}\r\n"
            "X-Powered-By: Karm Web\r\n"
            "\r\n",
            ct,
            co_try$(file.stat()).size));

        co_try$(stream.write(header.bytes()));
        co_try$(Io::copy(file, stream));
    }
}
