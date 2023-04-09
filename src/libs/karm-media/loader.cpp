
#include <jpeg/spec.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>
#include <png/spec.h>
#include <qoi/spec.h>

#include "font-ttf.h"
#include "loader.h"

namespace Karm::Media {

Res<Strong<Fontface>> loadFontface(Str path) {
    logInfo("media: loading '{}' as fontface...", path);

    auto file = try$(Sys::File::open(path));
    auto map = try$(Sys::mmap().map(file));
    Strong<Fontface> face = try$(TtfFontface::load(std::move(map)));
    return Ok(face);
}

Res<Font> loadFont(f64 size, Str path) {
    logInfo("media: loading '{}' as font...", path);

    return Ok(Font{
        .fontface = try$(loadFontface(path)),
        .fontsize = size,
    });
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

    img
        .mutPixels()
        .clip(img.bound())
        .clear(Gfx::PINK);

    // try$(png.decode(img));
    return Ok(img);
}

static Res<Image> loadJpeg(Bytes bytes) {
    auto jpeg = try$(Jpeg::Image::load(bytes));
    auto img = Image::alloc({jpeg.width(), jpeg.height()});

    img
        .mutPixels()
        .clip(img.bound())
        .clear(Gfx::PINK);

    // try$(jpeg.decode(img));
    return Ok(img);
}

Res<Image> loadImage(Str path) {
    auto file = try$(Sys::File::open(path));
    auto map = try$(Sys::mmap().map(file));

    if (Qoi::Image::isQoi(map.bytes())) {
        logInfo("media: loading '{}' as QOI...", path);
        return loadQoi(map.bytes());
    } else if (Png::Image::isPng(map.bytes())) {
        logInfo("media: loading '{}' as PNG...", path);
        return loadPng(map.bytes());
    } else if (Jpeg::Image::isJpeg(map.bytes())) {
        logInfo("media: loading '{}' as JPEG...", path);
        return loadJpeg(map.bytes());
    } else {
        logError("media: unknown image format for '{}'", path);
        return Error::invalidData("unknown image format");
    }
}

} // namespace Karm::Media
