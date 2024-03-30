#include <bmp/spec.h>
#include <jpeg/spec.h>
#include <png/spec.h>
#include <qoi/spec.h>
#include <ttf/spec.h>

#include "mime.h"

namespace Karm::Mime {

Opt<Mime> sniffBytes(Bytes bytes) {
    if (Jpeg::Image::isJpeg(bytes))
        return "image/jpeg"_mime;

    if (Png::Image::isPng(bytes))
        return "image/png"_mime;

    if (Ttf::Font::isTtf(bytes))
        return "font/ttf"_mime;

    if (Bmp::Image::isBmp(bytes))
        return "image/bmp"_mime;

    if (Qoi::Image::isQoi(bytes))
        return "image/qoi"_mime;

    return NONE;
}

struct {
    Str suffix;
    Str mime;
} SUFFIXES2MIME[] = {
    {"html", "text/html; charset=UTF-8"},
    {"css", "text/css; charset=UTF-8"},
    {"js", "application/javascript; charset=UTF-8"},
    {"pdf", "application/pdf"},
    {"png", "image/png"},
    {"jpg", "image/jpeg"},
    {"jpeg", "image/jpeg"},
    {"ico", "image/x-icon"},
    {"svg", "image/svg+xml"},
    {"woff", "font/woff"},
    {"woff2", "font/woff2"},
    {"ttf", "font/ttf"},
    {"eot", "application/vnd.ms-fontobject"},
    {"otf", "font/otf"},
    {"json", "application/json"},
    {"txt", "text/plain; charset=UTF-8"},
    {"mov", "video/quicktime"},
    {"mp4", "video/mp4"},
    {"webm", "video/webm"},
    {"exe", "application/x-msdownload"},
    {"zip", "application/zip"},
    {"tar", "application/x-tar"},
    {"gz", "application/gzip"},
    {"bz2", "application/x-bzip2"},
    {"7z", "application/x-7z-compressed"},
    {"rar", "application/vnd.rar"},
    {"xz", "application/x-xz"},
    {"webp", "image/webp"},
};

Opt<Mime> sniffSuffix(Str suffix) {
    for (auto const &s : SUFFIXES2MIME) {
        if (s.suffix == suffix)
            return s.mime;
    }
    return NONE;
}

struct {
    Str type;
    Str subtype;
    Mdi::Icon icon;
} MIME2ICON[] = {
    {"text", "", Mdi::FILE_DOCUMENT},
    {"text", "html", Mdi::LANGUAGE_HTML5},
    {"text", "css", Mdi::LANGUAGE_CSS3},
    {"text", "javascript", Mdi::LANGUAGE_JAVASCRIPT},
    {"text", "plain", Mdi::FILE_DOCUMENT},

    {"image", "", Mdi::IMAGE},
    {"image", "jpeg", Mdi::FILE_JPG_BOX},
    {"image", "png", Mdi::FILE_PNG_BOX},

    {"video", "", Mdi::FILMSTRIP},

    {"font", "", Mdi::FORMAT_FONT},

    {"application", "pdf", Mdi::FILE_PDF_BOX},
    {"application", "json", Mdi::CODE_JSON},
    {"application", "zip", Mdi::ZIP_BOX},
    {"application", "tar", Mdi::ZIP_BOX},
    {"application", "gz", Mdi::ZIP_BOX},
    {"application", "bz2", Mdi::ZIP_BOX},
    {"application", "7z-compressed", Mdi::ZIP_BOX},
    {"application", "rar", Mdi::ZIP_BOX},
    {"application", "x-xz", Mdi::ZIP_BOX},
    {"application", "x-msdownload", Mdi::COG_BOX},
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
