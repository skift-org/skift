#pragma once

#include <karm-gfx/buffer.h>
#include <karm-io/bscan.h>
#include <karm-mime/url.h>
#include <karm-mime/uti.h>

namespace Karm::Image {

struct Saver {
    Mime::Uti format = Mime::Uti::PUBLIC_BMP;

    static constexpr f64 MIN_QUALITY = 0.0;
    static constexpr f64 DEFAULT_QUALITY = 0.75;
    static constexpr f64 MAX_QUALITY = 1.0;
    f64 quality = DEFAULT_QUALITY;
};

Res<> save(Gfx::Pixels pixels, Io::BEmit& e, Saver const& props = {});

Res<> save(Gfx::Pixels pixels, Io::Writer& w, Saver const& props = {});

Res<> save(Gfx::Pixels pixels, Mime::Url const& url, Saver const& props = {});

} // namespace Karm::Image
