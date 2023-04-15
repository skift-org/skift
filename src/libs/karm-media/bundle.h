#pragma once

#include <karm-main/base.h>
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

#include "loader.h"

namespace Karm::Media {

struct Bundle :
    public Meta::NoCopy {

    virtual ~Bundle() = default;

    virtual Res<Sys::Mmap> load(Str id) = 0;

    Res<Strong<Fontface>> loadFontface(Str id) {
        return Media::loadFontface(try$(load(id)));
    }

    Res<Strong<Fontface>> loadFontfaceOrFallback(Str id) {
        if (auto result = loadFontface(id); result) {
            return result;
        }
        return Ok(Fontface::fallback());
    }

    Res<Font> loadFont(Str id, f64 size) {
        return Ok(Font{
            .fontface = try$(loadFontface(id)),
            .fontsize = size,
        });
    }

    Res<Font> loadFontOrFallback(Str id, f64 size) {
        if (auto result = loadFont(id, size); result) {
            return result;
        }
        return Ok(Font::fallback());
    }

    Res<Image> loadImage(Str id) {
        return Media::loadImage(try$(load(id)));
    }

    Res<Image> loadImageOrFallback(Str id) {
        if (auto result = loadImage(id); result) {
            return result;
        }
        return Ok(Image::fallback());
    }
};

struct DummyBundle :
    public Bundle {
    Res<Sys::Mmap> load(Str) override {
        return Error::other("dummy bundle");
    }
};

struct BundleHook : public Hook {
    Strong<Bundle> bundle;

    BundleHook(Strong<Bundle> bundle)
        : bundle(bundle) {}
};

} // namespace Karm::Media

inline Karm::Media::Bundle &useBundle(Ctx &ctx = Ctx::instance()) {
    return *ctx.use<Karm::Media::BundleHook>().bundle;
}
