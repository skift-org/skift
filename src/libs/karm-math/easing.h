#pragma once

#include <karm-math/const.h>
#include <karm-math/funcs.h>

namespace Karm::Math {

struct Easing {
    double (*_inner)(double);

    Easing()
        : _inner(linear) {}

    Easing(double (*inner)(double)) : _inner(inner) {}

    double operator()(double p) const {
        return _inner(p);
    }

    static double linear(double p) {
        return p;
    }

    static double quadraticIn(double p) {
        return p * p;
    }

    static double quadraticOut(double p) {
        return -(p * (p - 2));
    }

    static double quadraticInOut(double p) {
        if (p < 0.5) {
            return 2 * p * p;
        } else {
            return (-2 * p * p) + (4 * p) - 1;
        }
    }

    static double cubicIn(double p) {
        return p * p * p;
    }

    static double cubicOut(double p) {
        double f = (p - 1);
        return f * f * f + 1;
    }

    static double cubicInOut(double p) {
        if (p < 0.5) {
            return 4 * p * p * p;
        } else {
            double f = ((2 * p) - 2);
            return 0.5 * f * f * f + 1;
        }
    }

    static double quarticIn(double p) {
        return p * p * p * p;
    }

    static double quarticOut(double p) {
        double f = (p - 1);
        return f * f * f * (1 - p) + 1;
    }

    static double quarticInOut(double p) {
        if (p < 0.5) {
            return 8 * p * p * p * p;
        } else {
            double f = (p - 1);
            return -8 * f * f * f * f + 1;
        }
    }

    static double quinticIn(double p) {
        return p * p * p * p * p;
    }

    static double quinticOut(double p) {
        double f = (p - 1);
        return f * f * f * f * f + 1;
    }

    static double quinticInOut(double p) {
        if (p < 0.5) {
            return 16 * p * p * p * p * p;
        } else {
            double f = ((2 * p) - 2);
            return 0.5 * f * f * f * f * f + 1;
        }
    }

    static double sineIn(double p) {
        return sin((p - 1) * Math::TAU) + 1;
    }

    static double sineOut(double p) {
        return sin(p * Math::TAU);
    }

    static double sineInOut(double p) {
        return 0.5 * (1 - cos(p * Math::PI));
    }

    static double circularIn(double p) {
        return 1 - sqrt(1 - (p * p));
    }

    static double circularOut(double p) {
        return sqrt((2 - p) * p);
    }

    static double circularInOut(double p) {
        if (p < 0.5) {
            return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
        } else {
            return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
        }
    }

    static double exponentialIn(double p) {
        return (p == 0.0) ? p : pow(2, 10 * (p - 1));
    }

    static double exponentialOut(double p) {
        return (p == 1.0) ? p : 1 - pow(2, -10 * p);
    }

    static double exponentialInOut(double p) {
        if (p == 0.0 or p == 1.0) {
            return p;
        }

        if (p < 0.5) {
            return 0.5 * pow(2, (20 * p) - 10);
        } else {
            return -0.5 * pow(2, (-20 * p) + 10) + 1;
        }
    }

    static double elasticIn(double p) {
        return sin(13 * Math::TAU * p) * pow(2, 10 * (p - 1));
    }

    static double elasticOut(double p) {
        return sin(-13 * Math::TAU * (p + 1)) * pow(2, -10 * p) + 1;
    }

    static double elasticInOut(double p) {
        if (p < 0.5) {
            return 0.5 * sin(13 * Math::TAU * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
        } else {
            return 0.5 * (sin(-13 * Math::TAU * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
        }
    }

    static double backIn(double p) {
        return p * p * p - p * sin(p * Math::PI);
    }

    static double backOut(double p) {
        double f = (1 - p);
        return 1 - (f * f * f - f * sin(f * Math::PI));
    }

    static double backInOut(double p) {
        if (p < 0.5) {
            double f = 2 * p;
            return 0.5 * (f * f * f - f * sin(f * Math::PI));
        } else {
            double f = (1 - (2 * p - 1));
            return 0.5 * (1 - (f * f * f - f * sin(f * Math::PI))) + 0.5;
        }
    }

    static double bounceOut(double p) {
        if (p < 4 / 11.0) {
            return (121 * p * p) / 16.0;
        } else if (p < 8 / 11.0) {
            return (363 / 40.0 * p * p) - (99 / 10.0 * p) + 17 / 5.0;
        } else if (p < 9 / 10.0) {
            return (4356 / 361.0 * p * p) - (35442 / 1805.0 * p) + 16061 / 1805.0;
        } else {
            return (54 / 5.0 * p * p) - (513 / 25.0 * p) + 268 / 25.0;
        }
    }

    static double bounceIn(double p) {
        return 1 - bounceOut(1 - p);
    }

    static double bounceInOut(double p) {
        if (p < 0.5) {
            return 0.5 * bounceIn(p * 2);
        } else {
            return 0.5 * bounceOut(p * 2 - 1) + 0.5;
        }
    }
};

} // namespace Karm::Math
