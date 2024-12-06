#include <mdi/code-json.h>
#include <mdi/cog-box.h>
#include <mdi/file-document.h>
#include <mdi/file-jpg-box.h>
#include <mdi/file-pdf-box.h>
#include <mdi/file-png-box.h>
#include <mdi/file.h>
#include <mdi/filmstrip.h>
#include <mdi/format-font.h>
#include <mdi/image.h>
#include <mdi/language-css3.h>
#include <mdi/language-html5.h>
#include <mdi/language-javascript.h>
#include <mdi/zip-box.h>

#include "mime.h"

namespace Karm::Mime {

struct Suffix2Mime {
    Str suffix;
    Str mime;
};

static constexpr Array SUFFIXES2MIME = {
    Suffix2Mime{"html", "text/html; charset=UTF-8"},
    Suffix2Mime{"xhtml", "application/xhtml+xml; charset=UTF-8"},
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
    for (auto const &s : SUFFIXES2MIME) {
        if (s.suffix == suffix)
            return s.mime;
    }
    return NONE;
}

struct Mime2Icon {
    Str type;
    Str subtype;
    Mdi::Icon icon;
};

static constexpr Array MIME2ICON = {
    Mime2Icon{"text", "", Mdi::FILE_DOCUMENT},
    Mime2Icon{"text", "html", Mdi::LANGUAGE_HTML5},
    Mime2Icon{"text", "css", Mdi::LANGUAGE_CSS3},
    Mime2Icon{"text", "javascript", Mdi::LANGUAGE_JAVASCRIPT},
    Mime2Icon{"text", "plain", Mdi::FILE_DOCUMENT},

    Mime2Icon{"image", "", Mdi::IMAGE},
    Mime2Icon{"image", "jpeg", Mdi::FILE_JPG_BOX},
    Mime2Icon{"image", "png", Mdi::FILE_PNG_BOX},

    Mime2Icon{"video", "", Mdi::FILMSTRIP},

    Mime2Icon{"font", "", Mdi::FORMAT_FONT},

    Mime2Icon{"application", "pdf", Mdi::FILE_PDF_BOX},
    Mime2Icon{"application", "json", Mdi::CODE_JSON},
    Mime2Icon{"application", "zip", Mdi::ZIP_BOX},
    Mime2Icon{"application", "tar", Mdi::ZIP_BOX},
    Mime2Icon{"application", "gz", Mdi::ZIP_BOX},
    Mime2Icon{"application", "bz2", Mdi::ZIP_BOX},
    Mime2Icon{"application", "7z-compressed", Mdi::ZIP_BOX},
    Mime2Icon{"application", "rar", Mdi::ZIP_BOX},
    Mime2Icon{"application", "x-xz", Mdi::ZIP_BOX},
    Mime2Icon{"application", "x-msdownload", Mdi::COG_BOX},
};

Mdi::Icon iconFor(Mime const &mime) {
    Mdi::Icon icon = Mdi::FILE;

    for (auto const &m : MIME2ICON) {
        if (m.type == mime.type() and m.subtype == mime.subtype())
            return m.icon;

        if (m.type == mime.type() and m.subtype == "")
            icon = m.icon;
    }

    return icon;
}

} // namespace Karm::Mime
