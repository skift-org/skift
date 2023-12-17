#include <karm-io/funcs.h>
#include <karm-logger/logger.h>
#include <karm-main/main.h>
#include <karm-sys/file.h>
#include <karm-sys/socket.h>

Res<> entryPoint(Ctx &) {
    auto listener = try$(Sys::TcpListener::listen(Sys::Ip4::localhost(8080)));
    logInfo("Serving on http://{}", listener._addr);

    while (true) {
        auto stream = try$(listener.accept());

        logInfo("GET {}", stream._addr);

        Array<char, 4096> buf;
        auto len = try$(stream.read(mutBytes(buf)));

        Str request = {buf.buf(), len};
        logInfo("Request: {}", request);
        auto file = try$(Sys::File::open("bundle://server/public/index.html"_url));

        Io::StringWriter header;
        try$(Fmt::format(
            header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: {}\r\n"
            "X-Powered-By: Vaerk\r\n"
            "\r\n",
            try$(file.stat()).size));

        try$(stream.write(header.bytes()));
        try$(Io::copy(file, stream));
    }
}
