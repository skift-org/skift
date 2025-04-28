#include "mime.h"

namespace Karm::Mime {

struct Suffix2Mime {
    Str suffix;
    Str mime;
};

static constexpr Array SUFFIXES2MIME = {
    Suffix2Mime{"html", "text/html; charset=UTF-8"},
    Suffix2Mime{"xhtml", "application/xhtml+xml; charset=UTF-8"},
    Suffix2Mime{"xht", "application/xhtml+xml; charset=UTF-8"},
    Suffix2Mime{"css", "text/css; charset=UTF-8"},
    Suffix2Mime{"js", "application/javascript; charset=UTF-8"},
    Suffix2Mime{"pdf", "application/pdf"},

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

Opt<Mime> sniffSuffix(Str suffix) {
    for (auto const& s : SUFFIXES2MIME) {
        if (s.suffix == suffix)
            return s.mime;
    }
    return NONE;
}

} // namespace Karm::Mime
