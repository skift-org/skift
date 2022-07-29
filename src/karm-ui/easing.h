#pragma once

namespace Karm::Ui {

struct Easing {
    double (*_inner)(double);

    Easing(double (*inner)(double)) : _inner(inner) {}

    double operator()(double p) const {
        return _inner(p);
    }

    static double linear(double p);

    static double quadraticIn(double p);

    static double quadraticOut(double p);

    static double quadraticInOut(double p);

    static double cubicIn(double p);

    static double cubicOut(double p);

    static double cubicInOut(double p);

    static double quarticIn(double p);

    static double quarticOut(double p);

    static double quarticInOut(double p);

    static double quinticIn(double p);

    static double quinticOut(double p);

    static double quinticInOut(double p);

    static double sineIn(double p);

    static double sineOut(double p);

    static double sineInOut(double p);

    static double circularIn(double p);

    static double circularOut(double p);

    static double circularInOut(double p);

    static double exponentialIn(double p);

    static double exponentialOut(double p);

    static double exponentialInOut(double p);

    static double elasticIn(double p);

    static double elasticOut(double p);

    static double elasticInOut(double p);

    static double backIn(double p);

    static double backOut(double p);

    static double backInOut(double p);

    static double bounceOut(double p);

    static double bounceIn(double p);

    static double bounceInOut(double p);
};

} // namespace Karm::Ui
