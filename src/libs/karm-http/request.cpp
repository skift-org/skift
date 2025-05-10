module;

#include <karm-mime/url.h>

export module Karm.Http:request;

import :body;
import :header;
import :method;

namespace Karm::Http {

export struct Request {
    Method method;
    Mime::Url url;
    Version version;
    Header header;
    Opt<Rc<Body>> body;

    static Rc<Request> from(Http::Method method, Mime::Url url, Opt<Rc<Body>> body = NONE) {
        auto req = makeRc<Request>();

        req->method = method;
        req->url = url;
        req->header.add("Host", url.host);
        req->body = body;

        return req;
    }

    static Res<Request> parse(Io::SScan& s) {
        Request req;

        req.method = try$(parseMethod(s));

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        auto path = Mime::Path::parse(s, true, true);
        path.rooted = true;
        path.normalize();
        path.rooted = false;
        req.url.path = path;

        if (not s.skip(' '))
            return Error::invalidData("Expected space");

        req.version = try$(Version::parse(s));

        if (not s.skip("\r\n"))
            return Error::invalidData("Expected \"\\r\\n\"");

        try$(req.header.parse(s));

        return Ok(req);
    }

    Res<> unparse(Io::TextWriter& w) {
        // Start line
        url.path.rooted = true;
        try$(Io::format(w, "{} {} ", toStr(method), url.path));
        url.path.rooted = false;

        try$(version.unparse(w));
        try$(w.writeStr("\r\n"s));

        // Headers and empty line
        try$(header.unparse(w));
        try$(w.writeStr("\r\n"s));

        return Ok();
    }
};

} // namespace Karm::Http
