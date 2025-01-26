#include <karm-base/ring.h>
#include <karm-math/rand.h>

#include "filters.h"

namespace Karm::Gfx {

struct StackBlur {
    isize _radius;
    Ring<Math::Vec4u> _queue;
    Math::Vec4u _sum;

    StackBlur(isize radius)
        : _radius(radius), _queue(width()) {
        clear();
    }

    Math::Vec4u outgoingSum() const {
        Math::Vec4u sum = {};
        for (isize i = 0; i < _radius; i++) {
            sum = sum + _queue.peek(i);
        }
        return sum;
    }

    Math::Vec4u incomingSum() const {
        Math::Vec4u sum = {};
        for (isize i = 0; i < _radius; i++) {
            sum = sum + _queue.peek(width() - i - 1);
        }
        return sum;
    }

    isize width() const {
        return _radius * 2 + 1;
    }

    isize denominator() const {
        return _radius * (_radius + 2) - 1;
    }

    void enqueue(Math::Vec4u color) {
        _queue.pushBack(color);
        _sum = _sum + incomingSum() - outgoingSum();
    }

    Math::Vec4u dequeue() {
        auto res = _sum / denominator();
        _queue.popFront();
        return res;
    }

    void clear() {
        _sum = {};
        _queue.clear();
        for (isize i = 0; i < width(); i++) {
            _queue.pushBack({});
        }
    }
};

[[gnu::flatten]] void BlurFilter::apply(MutPixels p) const {
    if (amount == 0)
        return;

    StackBlur stack{(isize)amount};
    auto b = p.bound();

    for (isize y = b.top(); y < b.bottom(); y++) {
        for (isize i = 0; i < stack.width(); i++) {
            auto x = b.start() + i - (isize)amount;
            stack.dequeue();
            stack.enqueue(p.load({x, y}));
        }

        for (isize x = b.start(); x < b.end(); x++) {
            p.store({x, y}, stack.dequeue());
            stack.enqueue(p.load({x + (isize)amount + 1, y}));
        }

        stack.clear();
    }

    for (isize x = b.start(); x < b.end(); x++) {
        for (isize i = 0; i < stack.width(); i++) {
            isize const y = b.top() + i - (isize)amount;
            stack.dequeue();
            stack.enqueue(p.load({x, y}));
        }

        for (isize y = b.top(); y < b.bottom(); y++) {
            p.store({x, y}, stack.dequeue());
            stack.enqueue(p.load({x, y + (isize)amount + 1}));
        }

        stack.clear();
    }
}

void SaturationFilter::apply(MutPixels p) const {
    auto b = p.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = p.load({b.x + x, b.y + y});

            // weights from CCIR 601 spec
            // https://stackoverflow.com/questions/13806483/increase-or-decrease-color-saturation
            auto gray = 0.2989 * color.red + 0.5870 * color.green + 0.1140 * color.blue;

            u8 red = min(gray * amount + color.red * (1 - amount), 255);
            u8 green = min(gray * amount + color.green * (1 - amount), 255);
            u8 blue = min(gray * amount + color.blue * (1 - amount), 255);

            color = Color::fromRgba(red, green, blue, color.alpha);

            p.store({b.x + x, b.y + y}, color);
        }
    }
}

void GrayscaleFilter::apply(MutPixels p) const {
    auto b = p.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = p.load({b.x + x, b.y + y});

            // weights from CCIR 601 spec
            // https://stackoverflow.com/questions/13806483/increase-or-decrease-color-saturation
            f64 gray = 0.2989 * color.red + 0.5870 * color.green + 0.1140 * color.blue;
            color = Color::fromRgba(gray, gray, gray, color.alpha);
            p.store({b.x + x, b.y + y}, color);
        }
    }
}

void ContrastFilter::apply(MutPixels p) const {
    auto b = p.bound();

    f64 const factor = (259 * ((amount * 255) + 255)) / (255 * (259 - (amount * 255)));

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = p.load({b.x + x, b.y + y});

            color = Color::fromRgba(
                clamp(factor * (color.red - 128) + 128, 0, 255),
                clamp(factor * (color.green - 128) + 128, 0, 255),
                clamp(factor * (color.blue - 128) + 128, 0, 255),
                color.alpha
            );

            p.store({b.x + x, b.y + y}, color);
        }
    }
}

void BrightnessFilter::apply(MutPixels p) const {
    if (Math::epsilonEq(amount, 1.0))
        return;

    auto b = p.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = p.load({b.x + x, b.y + y});

            color = Color::fromRgba(
                min(color.red * amount, 255),
                min(color.green * amount, 255),
                min(color.blue * amount, 255),
                color.alpha
            );

            p.store({b.x + x, b.y + y}, color);
        }
    }
}

void NoiseFilter::apply(MutPixels p) const {
    Math::Rand rand{0x12341234};
    u8 alpha = 255 * amount;
    auto b = p.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            u8 noise = rand.nextU8();

            p.blend(
                {b.x + x, b.y + y},
                Color::fromRgba(noise, noise, noise, alpha)
            );
        }
    }
}

void SepiaFilter::apply(MutPixels p) const {
    if (Math::epsilonEq(amount, 0.0))
        return;

    auto b = p.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = p.load({b.x + x, b.y + y});

            auto sepiaColor = Color::fromRgba(
                min((color.red * 0.393) + (color.green * 0.769) + (color.blue * 0.189), 255u),
                min((color.red * 0.349) + (color.green * 0.686) + (color.blue * 0.168), 255u),
                min((color.red * 0.272) + (color.green * 0.534) + (color.blue * 0.131), 255u),
                color.alpha
            );

            p.store({b.x + x, b.y + y}, color.lerpWith(sepiaColor, amount));
        }
    }
}

void TintFilter::apply(MutPixels p) const {
    auto b = p.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            auto color = p.load({b.x + x, b.y + y});

            auto tintColor = Color::fromRgba(
                (color.red * amount.red) / 255,
                (color.green * amount.green) / 255,
                (color.blue * amount.blue) / 255,
                (color.alpha * amount.alpha) / 255
            );

            p.store({b.x + x, b.y + y}, tintColor);
        }
    }
}

void OverlayFilter::apply(MutPixels p) const {
    auto b = p.bound();

    for (isize y = 0; y < b.height; y++) {
        for (isize x = 0; x < b.width; x++) {
            p.blend(
                {b.x + x, b.y + y},
                amount
            );
        }
    }
}

void FilterChain::apply(MutPixels p) const {
    filters.visit([&](auto& f) {
        f->apply(p);
        return true;
    });
}

} // namespace Karm::Gfx
