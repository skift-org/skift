#include <karm-sys/file.h>

#include "bmp/encoder.h"
#include "jpeg/encoder.h"
#include "qoi/encoder.h"
#include "tga/encoder.h"

//
#include "saver.h"

namespace Karm::Image {

Res<> save(Gfx::Pixels pixels, Io::BEmit& e, Saver const& props) {
    if (props.format == Mime::Uti::PUBLIC_BMP) {
        return Bmp::encode(pixels, e);
    } else if (props.format == Mime::Uti::PUBLIC_TGA) {
        return Tga::encode(pixels, e);
    } else if (props.format == Mime::Uti::PUBLIC_JPEG) {
        return Jpeg::encode(pixels, e);
    } else if (props.format == Mime::Uti::PUBLIC_QOI) {
        return Qoi::encode(pixels, e);
    } else {
        return Error::invalidData("unsupported image format");
    }
}

Res<> save(Gfx::Pixels pixels, Io::Writer& w, Saver const& props) {
    Io::BEmit e{w};
    return save(pixels, e, props);
}

Res<> save(Gfx::Pixels pixels, Mime::Url const& url, Saver const& props) {
    auto file = try$(Sys::File::create(url));
    Io::BEmit e{file};
    return save(pixels, e, props);
}

} // namespace Karm::Image
