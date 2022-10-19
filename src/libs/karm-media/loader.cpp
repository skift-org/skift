
#include <jpeg/spec.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>
#include <png/spec.h>
#include <qoi/spec.h>

#include "font-ttf.h"
#include "loader.h"

namespace Karm::Media {

Result<Strong<Fontface>> loadFontface(Str path) {
    auto file = try$(Sys::File::open(path));
    auto map = try$(Sys::mmap().map(file));
    Strong<Fontface> face = try$(TtfFontface::load(std::move(map)));
    return face;
}

Result<Font> loadFont(double size, Str path) {
    return Font{size, try$(loadFontface(path))};
}

static Result<Image> loadQoi(Bytes bytes) {
    auto qoi = try$(Qoi::Image::load(bytes));
    Image image{Gfx::RGBA8888, {qoi.width(), qoi.height()}};
    try$(qoi.decode(image));
    return image;
}

static Result<Image> loadPng(Bytes bytes) {
    auto png = try$(Png::Image::load(bytes));
    Image image{Gfx::RGBA8888, {png.width(), png.height()}};

    Gfx::Surface surface = image;
    surface.clearRect(surface.bound(), Gfx::PINK);
    // try$(png.decode(image));
    return image;
}

static Result<Image> loadJpeg(Bytes bytes) {
    auto jpeg = try$(Jpeg::Image::load(bytes));
    Image image{Gfx::RGBA8888, {jpeg.width(), jpeg.height()}};

    Gfx::Surface surface = image;
    surface.clearRect(surface.bound(), Gfx::PINK);
    // try$(jpeg.decode(image));
    return image;
}

Result<Image> loadImage(Str path) {
    auto file = try$(Sys::File::open(path));
    auto map = try$(Sys::mmap().map(file));

    if (Qoi::Image::isQoi(map.bytes())) {
        Debug::linfo("Loading '{}' as QOI...", path);
        return loadQoi(map.bytes());
    } else if (Png::Image::isPng(map.bytes())) {
        Debug::linfo("Loading '{}' as PNG...", path);
        return loadPng(map.bytes());
    } else if (Jpeg::Image::isJpeg(map.bytes())) {
        Debug::linfo("Loading '{}' as JPEG...", path);
        return loadJpeg(map.bytes());
    } else {
        Debug::lerror("Unknown image format for '{}'", path);
        return Error{"unknown image format"};
    }
}

} // namespace Karm::Media
