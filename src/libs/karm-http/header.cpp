module;

#include <karm-io/aton.h>
#include <karm-io/expr.h>
#include <karm-io/fmt.h>
#include <karm-mime/mime.h>

export module Karm.Http:header;

namespace Karm::Http {

export struct Version {
    u8 major;
    u8 minor;

    static Res<Version> parse(Io::SScan& s) {
        if (not s.skip("HTTP/"))
            return Error::invalidData("Expected \"HTTP/\"");
        Version v;
        v.major = try$(atou(s));
        s.skip('.');
        v.minor = try$(atou(s));
        return Ok(v);
    }

    Res<> unparse(Io::TextWriter& w) {
        try$(Io::format(w, "HTTP/{}.{}", major, minor));
        return Ok();
    }

    bool operator==(Version const& other) const = default;

    auto operator<=>(Version const& other) const = default;
};

export struct Header : public Map<String, String> {
    using Map<String, String>::Map;

    void add(Str const& key, Str value) {
        put(key, std::move(value));
    }

    Res<> parse(Io::SScan& s) {
        while (not s.ended()) {
            Str key;
            Str value;

            auto RE_ENDLINE =
                Re::zeroOrMore(' '_re) & "\r\n"_re;

            auto RE_SEPARATOR =
                Re::separator(':'_re);

            auto RE_KEY_VALUE =
                Re::token(
                    key,
                    Re::until(RE_SEPARATOR)
                ) &
                RE_SEPARATOR &
                Re::token(
                    value,
                    Re::until(RE_ENDLINE)
                ) &
                RE_ENDLINE;

            if (s.skip("\r\n"))
                break;

            if (not s.skip(RE_KEY_VALUE))
                return Error::invalidData("Expected header");

            put(key, value);
        }

        return Ok();
    }

    Res<> unparse(Io::TextWriter& w) {
        for (auto& [key, value] : iter()) {
            try$(Io::format(w, "{}: {}\r\n", key, value));
        }

        try$(w.writeStr("\r\n"s));

        return Ok();
    }

    Opt<usize> contentLength() {
        if (auto value = tryGet("Content-Length"s))
            return Io::atou(value->str());
        return NONE;
    }

    Opt<Mime::Mime> contentType() {
        if (auto value = tryGet("Content-Type"s))
            return Mime::Mime{value->str()};
        return NONE;
    }
};

} // namespace Karm::Http

template <>
struct Karm::Io::Formatter<Karm::Http::Version> {
    Res<> format(Io::TextWriter& writer, Karm::Http::Version version) {
        return Io::format(writer, "HTTP/{}.{}", version.major, version.minor);
    }
};
