#pragma once

#include <karm-gfx/buffer.h>
#include <karm-meta/nocopy.h>

namespace Karm::Image {

struct Picture {
    Rc<Gfx::Surface const> _surface;

    Picture(Rc<Gfx::Surface const> surface)
        : _surface(std::move(surface)) {}

    Picture(Rc<Gfx::Surface> surface)
        : _surface(std::move(surface)) {}

    always_inline operator Gfx::Pixels() const {
        return pixels();
    }

    always_inline Gfx::Pixels pixels() const {
        return _surface->pixels();
    }

    always_inline isize width() const {
        return _surface->width();
    }

    always_inline isize height() const {
        return _surface->height();
    }

    always_inline Math::Recti bound() const {
        return {0, 0, width(), height()};
    }

    always_inline Gfx::Color sample(Math::Vec2f pos) const {
        return pixels().sample(pos);
    }

    void repr(Io::Emit& e) const {
        e("(picture {}x{})", width(), height());
    }
};

} // namespace Karm::Image
