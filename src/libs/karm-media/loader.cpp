
#include <bmp/spec.h>
#include <jpeg/spec.h>
#include <karm-sys/chan.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>
#include <karm-sys/time.h>
#include <png/spec.h>
#include <qoi/spec.h>

#include "font-ttf.h"
#include "loader.h"

namespace Karm::Media {

/* --- Font loading --------------------------------------------------------- */

Res<Strong<Fontface>> loadFontface(Sys::Mmap &&map) {
    return Ok(try$(TtfFontface::load(std::move(map))));
}

Res<Strong<Fontface>> loadFontface(Url::Url url) {
    logInfo("media: loading '{}' as fontface...", url);
    auto file = try$(Sys::File::open(url));
    auto map = try$(Sys::mmap().map(file));
    return loadFontface(std::move(map));
}

Res<Strong<Fontface>> loadFontfaceOrFallback(Url::Url url) {
    if (auto result = loadFontface(url); result) {
        return result;
    }
    return Ok(Fontface::fallback());
}

Res<Font> loadFont(f64 size, Url::Url url) {
    logInfo("media: loading '{}' as font...", url);

    return Ok(Font{
        .fontface = try$(loadFontface(url)),
        .fontsize = size,
    });
}

Res<Font> loadFontOrFallback(f64 size, Url::Url url) {
    if (auto result = loadFont(size, url); result) {
        return result;
    }
    return Ok(Font::fallback());
}

/* --- Image loading -------------------------------------------------------- */

static Res<Image> loadBmp(Bytes bytes) {
    auto bmp = try$(Bmp::Image::load(bytes));
    Io::Emit e{Sys::out()};
    bmp.dump(e);
    auto img = Image::alloc({bmp.width(), bmp.height()});
    try$(bmp.decode(img));

    return Ok(img);
}

static Res<Image> loadQoi(Bytes bytes) {
    auto qoi = try$(Qoi::Image::load(bytes));
    auto img = Image::alloc({qoi.width(), qoi.height()});
    try$(qoi.decode(img.mutPixels()));
    return Ok(img);
}

static Res<Image> loadPng(Bytes bytes) {
    auto png = try$(Png::Image::load(bytes));
    auto img = Image::alloc({png.width(), png.height()});
    try$(png.decode(img));
    return Ok(img);
}

static Res<Image> loadJpeg(Bytes bytes) {
    auto jpeg = try$(Jpeg::Image::load(bytes));
    auto img = Image::alloc({jpeg.width(), jpeg.height()});
    try$(jpeg.decode(img));
    return Ok(img);
}

Res<Image> loadImage(Sys::Mmap &&map) {
    if (Bmp::Image::isBmp(map.bytes())) {
        return loadBmp(map.bytes());
    } else if (Qoi::Image::isQoi(map.bytes())) {
        return loadQoi(map.bytes());
    } else if (Png::Image::isPng(map.bytes())) {
        return loadPng(map.bytes());
    } else if (Jpeg::Image::isJpeg(map.bytes())) {
        return loadJpeg(map.bytes());
    } else {
        return Error::invalidData("unknown image format");
    }
}

Res<Image> loadImage(Url::Url url) {
    auto file = try$(Sys::File::open(url));
    auto map = try$(Sys::mmap().map(file));
    return loadImage(std::move(map));
}

Res<Image> loadImageOrFallback(Url::Url url) {
    if (auto result = loadImage(url); result) {
        return result;
    }
    return Ok(Image::fallback());
}

} // namespace Karm::Media
