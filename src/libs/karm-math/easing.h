#pragma once

import Karm.Core;

namespace Karm::Math {

struct Easing {
    f64 (*_inner)(f64);

    Easing()
        : _inner(linear) {}

    Easing(f64 (*inner)(f64)) : _inner(inner) {}

    f64 operator()(f64 p) const {
        return _inner(p);
    }

    static f64 linear(f64 p) {
        return p;
    }

    static f64 quadraticIn(f64 p) {
        return p * p;
    }

    static f64 quadraticOut(f64 p) {
        return -(p * (p - 2));
    }

    static f64 quadraticInOut(f64 p) {
        if (p < 0.5) {
            return 2 * p * p;
        }

        return (-2 * p * p) + (4 * p) - 1;
    }

    static f64 cubicIn(f64 p) {
        return p * p * p;
    }

    static f64 cubicOut(f64 p) {
        f64 f = (p - 1);
        return f * f * f + 1;
    }

    static f64 cubicInOut(f64 p) {
        if (p < 0.5) {
            return 4 * p * p * p;
        }
        f64 f = ((2 * p) - 2);
        return 0.5 * f * f * f + 1;
    }

    static f64 quarticIn(f64 p) {
        return p * p * p * p;
    }

    static f64 quarticOut(f64 p) {
        f64 f = (p - 1);
        return f * f * f * (1 - p) + 1;
    }

    static f64 quarticInOut(f64 p) {
        if (p < 0.5) {
            return 8 * p * p * p * p;
        }
        f64 f = (p - 1);
        return -8 * f * f * f * f + 1;
    }

    static f64 quinticIn(f64 p) {
        return p * p * p * p * p;
    }

    static f64 quinticOut(f64 p) {
        f64 f = (p - 1);
        return f * f * f * f * f + 1;
    }

    static f64 quinticInOut(f64 p) {
        if (p < 0.5) {
            return 16 * p * p * p * p * p;
        }
        f64 f = ((2 * p) - 2);
        return 0.5 * f * f * f * f * f + 1;
    }

    static f64 sineIn(f64 p) {
        return sin((p - 1) * Math::TAU) + 1;
    }

    static f64 sineOut(f64 p) {
        return sin(p * Math::TAU);
    }

    static f64 sineInOut(f64 p) {
        return 0.5 * (1 - cos(p * Math::PI));
    }

    static f64 circularIn(f64 p) {
        return 1 - sqrt(1 - (p * p));
    }

    static f64 circularOut(f64 p) {
        return sqrt((2 - p) * p);
    }

    static f64 circularInOut(f64 p) {
        if (p < 0.5) {
            return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
        }
        return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
    }

    static f64 exponentialIn(f64 p) {
        return (p == 0.0) ? p : pow<f64>(2, 10 * (p - 1));
    }

    static f64 exponentialOut(f64 p) {
        return (p == 1.0) ? p : 1 - pow<f64>(2, -10 * p);
    }

    static f64 exponentialInOut(f64 p) {
        if (p == 0.0 or p == 1.0) {
            return p;
        }

        if (p < 0.5) {
            return 0.5 * pow<f64>(2, (20 * p) - 10);
        }

        return -0.5 * pow<f64>(2, (-20 * p) + 10) + 1;
    }

    static f64 elasticIn(f64 p) {
        return sin(13 * Math::TAU * p) * pow<f64>(2, 10 * (p - 1));
    }

    static f64 elasticOut(f64 p) {
        return sin(-13 * Math::TAU * (p + 1)) * pow<f64>(2, -10 * p) + 1;
    }

    static f64 elasticInOut(f64 p) {
        if (p < 0.5) {
            return 0.5 * sin(13 * Math::TAU * (2 * p)) * pow<f64>(2, 10 * ((2 * p) - 1));
        }

        return 0.5 * (sin(-13 * Math::TAU * ((2 * p - 1) + 1)) * pow<f64>(2, -10 * (2 * p - 1)) + 2);
    }

    static f64 backIn(f64 p) {
        return p * p * p - p * sin(p * Math::PI);
    }

    static f64 backOut(f64 p) {
        f64 f = (1 - p);
        return 1 - (f * f * f - f * sin(f * Math::PI));
    }

    static f64 backInOut(f64 p) {
        if (p < 0.5) {
            f64 f = 2 * p;
            return 0.5 * (f * f * f - f * sin(f * Math::PI));
        }

        f64 f = (1 - (2 * p - 1));

        return 0.5 * (1 - (f * f * f - f * sin(f * Math::PI))) + 0.5;
    }

    static f64 bounceOut(f64 p) {
        if (p < 4 / 11.0) {
            return (121 * p * p) / 16.0;
        }

        if (p < 8 / 11.0) {
            return (363 / 40.0 * p * p) - (99 / 10.0 * p) + 17 / 5.0;
        }

        if (p < 9 / 10.0) {
            return (4356 / 361.0 * p * p) - (35442 / 1805.0 * p) + 16061 / 1805.0;
        }

        return (54 / 5.0 * p * p) - (513 / 25.0 * p) + 268 / 25.0;
    }

    static f64 bounceIn(f64 p) {
        return 1 - bounceOut(1 - p);
    }

    static f64 bounceInOut(f64 p) {
        if (p < 0.5) {
            return 0.5 * bounceIn(p * 2);
        }

        return 0.5 * bounceOut(p * 2 - 1) + 0.5;
    }
};

} // namespace Karm::Math
