#include <karm-io/funcs.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>
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

Res<> entryPoint(Ctx &) {
    auto listener = try$(Sys::TcpListener::listen(Sys::Ip4::localhost(8080)));
    logInfo("Serving on http://{}", listener._addr);

    while (true) {
        auto stream = try$(listener.accept());

        Array<char, 4096> buf;
        auto len = try$(stream.read(mutBytes(buf)));

        Str reqStr{buf.buf(), len};
        Io::SScan reqScan{reqStr};
        auto req = try$(Web::Http::Request::parse(reqScan));
        auto fileUrl = "bundle://web-server/public/"_url / req.path;
        auto ct = contentType(req.path);
        logInfo("{}: {} {} -> {} ({})", stream._addr, req.method, req.path, fileUrl, ct);
        auto file = try$(Sys::File::open(fileUrl));

        Io::StringWriter header;
        try$(Fmt::format(
            header,
            "HTTP/1.1 200 OK\r\n"
            "Connection: close\r\n"
            "Content-Type: {}\r\n"
            "Content-Length: {}\r\n"
            "X-Powered-By: Karm Web\r\n"
            "\r\n",
            ct,
            try$(file.stat()).size));

        try$(stream.write(header.bytes()));
        try$(Io::copy(file, stream));
    }
}
