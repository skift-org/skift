module;

#include <karm-gfx/canvas.h>

export module Karm.Scene:image;

import Karm.Core;
import Karm.Image;
import :node;

namespace Karm::Scene {

export struct Image : Node {
    Math::Rectf _bound;
    Karm::Image::Picture _picture;
    Math::Radiif _radii;

    Image(Math::Rectf bound, Karm::Image::Picture picture, Math::Radiif radii = {})
        : _bound(bound), _picture(std::move(picture)), _radii(radii) {
    }

    Math::Rectf bound() override {
        return _bound;
    }

    void paint(Gfx::Canvas& ctx, Math::Rectf r, PaintOptions) override {
        if (not r.colide(bound()))
            return;

        if (not _radii.zero()) {
            ctx.fillStyle(_picture.pixels());
            ctx.fill(bound(), _radii);
        } else {
            ctx.blit(_bound.cast<isize>(), _picture.pixels());
        }
    }

    void repr(Io::Emit& e) const override {
        e("(image z:{})", zIndex);
    }
};

} // namespace Karm::Scene
