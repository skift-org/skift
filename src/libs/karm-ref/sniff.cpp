module;

#include <karm-core/macros.h>

export module Karm.Ref:sniff;

import :mime;

namespace Karm::Ref {

// MARK: Sniff Suffix ----------------------------------------------------------

struct Suffix2Mime {
    Str suffix;
    Str mime;
};

constexpr Array SUFFIXES2MIME = {
    Suffix2Mime{"html", "text/html; charset=UTF-8"},
    Suffix2Mime{"xhtml", "application/xhtml+xml; charset=UTF-8"},
    Suffix2Mime{"xht", "application/xhtml+xml; charset=UTF-8"},
    Suffix2Mime{"css", "text/css; charset=UTF-8"},
    Suffix2Mime{"js", "application/javascript; charset=UTF-8"},
    Suffix2Mime{"pdf", "application/pdf"},
    Suffix2Mime{"md", "text/markdown"},

    Suffix2Mime{"bmp", "image/bmp"},
    Suffix2Mime{"png", "image/png"},
    Suffix2Mime{"jpg", "image/jpeg"},
    Suffix2Mime{"jpeg", "image/jpeg"},
    Suffix2Mime{"ico", "image/x-icon"},
    Suffix2Mime{"svg", "image/svg+xml"},

    Suffix2Mime{"woff", "font/woff"},
    Suffix2Mime{"woff2", "font/woff2"},
    Suffix2Mime{"ttf", "font/ttf"},
    Suffix2Mime{"eot", "application/vnd.ms-fontobject"},
    Suffix2Mime{"otf", "font/otf"},
    Suffix2Mime{"json", "application/json"},
    Suffix2Mime{"txt", "text/plain; charset=UTF-8"},
    Suffix2Mime{"xml", "text/xml; charset=UTF-8"},
    Suffix2Mime{"mov", "video/quicktime"},
    Suffix2Mime{"mp4", "video/mp4"},
    Suffix2Mime{"webm", "video/webm"},
    Suffix2Mime{"exe", "application/x-msdownload"},
    Suffix2Mime{"zip", "application/zip"},
    Suffix2Mime{"tar", "application/x-tar"},
    Suffix2Mime{"gz", "application/gzip"},
    Suffix2Mime{"bz2", "application/x-bzip2"},
    Suffix2Mime{"7z", "application/x-7z-compressed"},
    Suffix2Mime{"rar", "application/vnd.rar"},
    Suffix2Mime{"xz", "application/x-xz"},
    Suffix2Mime{"webp", "image/webp"},
};

export Opt<Mime> sniffSuffix(Str suffix) {
    for (auto const& s : SUFFIXES2MIME) {
        if (s.suffix == suffix)
            return s.mime;
    }
    return NONE;
}

// MARK: Sniff Bytes -----------------------------------------------------------

struct MimePattern {
    Io::BPattern pattern;
    Vec<u8> leading;
    Vec<u8> terminating;
    Mime mime;

    bool match(Bytes bytes) const {
        Io::BScan s{bytes};

        while (not s.ended() and contains(leading, s.peekU8le())) {
            s.nextU8le();
        }

        if (pattern.match(s.remBytes()).v0 != Match::YES) {
            return false;
        }

        s.skip(pattern.len());

        if (terminating.len()) {
            if (s.ended() or not contains(terminating, s.peekU8le())) {
                return false;
            }
        }
        return true;
    }
};

bool _isBinaryData(Bytes const bytes) {
    for (u8 byte : bytes) {
        if ((byte <= 0x08) ||
            (byte == 0x0B) ||
            (byte >= 0x0E && byte <= 0x1A) ||
            (byte >= 0x1C && byte <= 0x1F)) {
            return true;
        }
    }
    return false;
}

// https://mimesniff.spec.whatwg.org/#identifying-a-resource-with-an-unknown-mime-type
export Mime sniffBytes(Bytes bytes) {
    static Vec<MimePattern> patterns = {
        {Io::BPattern::from("3C 21 44 4F 43 54 59 50 45 20 48 54 4D 4C", "FF FF DF DF DF DF DF DF DF FF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 48 54 4D 4C", "FF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 48 45 41 44", "FF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 53 43 52 49 50 54", "FF DF DF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 49 46 52 41 4D 45", "FF DF DF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 48 31", "FF DF FF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 44 49 56", "FF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 46 4F 4E 54", "FF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 54 41 42 4C 45", "FF DF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 41", "FF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 53 54 59 4C 45", "FF DF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 54 49 54 4C 45", "FF DF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 42", "FF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 42 4F 44 59", "FF DF DF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 42 52", "FF DF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 50", "FF DF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 21 2D 2D", "FF FF FF FF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/html"_mime},
        {Io::BPattern::from("3C 3F 78 6D 6C", "FF FF FF FF FF"), {' ', '\t', '\n', '\r'}, {' ', '>'}, "text/xml"_mime},
        {Io::BPattern::from("25 50 44 46 2D", "FF FF FF FF FF"), {}, {}, "application/pdf"_mime},

        {Io::BPattern::from("25 21 50 53 2D 41 64 6F 62 65 2D", "FF FF FF FF FF FF FF FF FF FF FF"), {}, {}, "application/postscript"_mime},
        {Io::BPattern::from("FE FF 00 00", "FF FF 00 00"), {}, {}, "text/plain"_mime}, // UTF-16BE BOM
        {Io::BPattern::from("FF FE 00 00", "FF FF 00 00"), {}, {}, "text/plain"_mime}, // UTF-16LE BOM
        {Io::BPattern::from("EF BB BF 00", "FF FF FF 00"), {}, {}, "text/plain"_mime}, // UTF-8 BOM

        // https://mimesniff.spec.whatwg.org/#image-type-pattern-matching-algorithm
        {Io::BPattern::from("00 00 01 00", "FF FF FF FF"), {}, {}, "image/x-icon"_mime},                                                           // ICO signature
        {Io::BPattern::from("00 00 02 00", "FF FF FF FF"), {}, {}, "image/x-icon"_mime},                                                           // CUR signature
        {Io::BPattern::from("42 4D", "FF FF"), {}, {}, "image/bmp"_mime},                                                                          // BMP signature ("BM")
        {Io::BPattern::from("47 49 46 38 37 61", "FF FF FF FF FF FF"), {}, {}, "image/gif"_mime},                                                  // GIF87a
        {Io::BPattern::from("47 49 46 38 39 61", "FF FF FF FF FF FF"), {}, {}, "image/gif"_mime},                                                  // GIF89a
        {Io::BPattern::from("52 49 46 46 00 00 00 00 57 45 42 50 56 50", "FF FF FF FF 00 00 00 00 FF FF FF FF FF FF"), {}, {}, "image/webp"_mime}, // WEBP ("RIFF....WEBPVP")
        {Io::BPattern::from("89 50 4E 47 0D 0A 1A 0A", "FF FF FF FF FF FF FF FF"), {}, {}, "image/png"_mime},                                      // PNG
        {Io::BPattern::from("FF D8 FF", "FF FF FF"), {}, {}, "image/jpeg"_mime},

        // https://mimesniff.spec.whatwg.org/#matching-an-archive-type-pattern
        {Io::BPattern::from("1F 8B 08", "FF FF FF"), {}, {}, "application/x-gzip"_mime},                                   // GZIP signature
        {Io::BPattern::from("50 4B 03 04", "FF FF FF FF"), {}, {}, "application/zip"_mime},                                // ZIP signature "PK\x03\x04"
        {Io::BPattern::from("52 61 72 20 1A 07 00", "FF FF FF FF FF FF FF"), {}, {}, "application/x-rar-compressed"_mime}, // RAR signature "Rar \x1A\x07\x00"

        // https://mimesniff.spec.whatwg.org/#matching-a-font-type-pattern
        {
            Io::BPattern::from(
                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 4C 50",
                "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF FF"
            ),
            {},
            {},
            "application/vnd.ms-fontobject"_mime,
        },                                                                                  // EOT signature
        {Io::BPattern::from("00 01 00 00", "FF FF FF FF"), {}, {}, "font/ttf"_mime},        // TrueType signature
        {Io::BPattern::from("4F 54 54 4F", "FF FF FF FF"), {}, {}, "font/otf"_mime},        // OpenType "OTTO"
        {Io::BPattern::from("74 74 63 66", "FF FF FF FF"), {}, {}, "font/collection"_mime}, // TrueType collection "ttcf"
        {Io::BPattern::from("77 4F 46 46", "FF FF FF FF"), {}, {}, "font/woff"_mime},       // WOFF 1.0 "wOFF"
        {Io::BPattern::from("77 4F 46 32", "FF FF FF FF"), {}, {}, "font/woff2"_mime},      // WOFF 2.0 "wOF2"
    };

    for (auto p : patterns) {
        if (p.match(bytes))
            return p.mime;
    }

    // 9. If resource’s resource header contains no binary data bytes, return "text/plain".
    if (not _isBinaryData(bytes)) {
        return "text/plain"_mime;
    }

    // 10. Return "application/octet-stream".
    return "application/octet-stream"_mime;
}

export Res<Mime> sniffReader(Io::Reader& reader) {
    Array<u8, 1445> header;
    auto len = try$(reader.read(header));
    return Ok(sniffBytes(sub(header, 0, len)));
}

} // namespace Karm::Ref
