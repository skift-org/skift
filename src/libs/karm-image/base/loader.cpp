module;

#include <karm-gfx/buffer.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

export module Karm.Image:base.loader;

import :base.picture;
import :bmp.decoder;
import :gif.decoder;
import :jpeg.decoder;
import :png.decoder;
import :qoi.decoder;
import :tga.decoder;

namespace Karm::Image {

namespace {

Res<Picture> loadBmp(Bytes bytes) {
    auto bmp = try$(Bmp::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({bmp.width(), bmp.height()});
    try$(bmp.decode(*img));
    return Ok(img);
}

Res<Picture> loadQoi(Bytes bytes) {
    auto qoi = try$(Qoi::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({qoi.width(), qoi.height()});
    try$(qoi.decode(*img));
    return Ok(img);
}

Res<Picture> loadPng(Bytes bytes) {
    auto png = try$(Png::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({png.width(), png.height()});
    try$(png.decode(*img));
    return Ok(img);
}

Res<Picture> loadJpeg(Bytes bytes) {
    auto jpeg = try$(Jpeg::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({jpeg.width(), jpeg.height()});
    try$(jpeg.decode(*img));
    return Ok(img);
}

Res<Picture> loadTga(Bytes bytes) {
    auto tga = try$(Tga::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({tga.width(), tga.height()});
    try$(tga.decode(*img));
    return Ok(img);
}

Res<Picture> loadGif(Bytes bytes) {
    auto gif = try$(Gif::Decoder::init(bytes));
    auto img = Gfx::Surface::alloc({gif.width(), gif.height()});
    try$(gif.decode(*img));
    return Ok(img);
}

} // namespace

export Res<Picture> load(Bytes bytes) {
    if (Bmp::Decoder::sniff(bytes)) {
        return loadBmp(bytes);
    } else if (Qoi::Decoder::sniff(bytes)) {
        return loadQoi(bytes);
    } else if (Png::Decoder::sniff(bytes)) {
        return loadPng(bytes);
    } else if (Jpeg::Decoder::sniff(bytes)) {
        return loadJpeg(bytes);
    } else if (Tga::Decoder::sniff(bytes)) {
        return loadTga(bytes);
    } else if (Gif::Decoder::sniff(bytes)) {
        return loadGif(bytes);
    } else {
        return Error::invalidData("unknown image format");
    }
}

export Res<Picture> load(Ref::Url url) {
    if (url.scheme == "data") {
        auto blob = try$(url.blob);
        return load(blob->data);
    }
    auto file = try$(Sys::File::open(url));
    auto map = try$(Sys::mmap().map(file));
    return load(map.bytes());
}

export Res<Picture> loadOrFallback(Ref::Url url) {
    if (auto result = load(url); result)
        return result;
    return Ok(Gfx::Surface::fallback());
}

} // namespace Karm::Image
