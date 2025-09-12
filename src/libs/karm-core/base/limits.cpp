export module Karm.Core:base.limits;

import :base.base;

namespace Karm {

export template <typename T>
struct Limits {
};

export template <>
struct Limits<bool> {
    static constexpr bool MIN = false;
    static constexpr bool MAX = true;
    static constexpr bool IS_SIGNED = false;
    static constexpr usize DIGITS = 1;
    static constexpr bool BITS = 1;
};

export template <>
struct Limits<signed char> {
    static constexpr signed char MIN = -__SCHAR_MAX__ - 1;
    static constexpr signed char MAX = __SCHAR_MAX__;
    static constexpr bool SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ - 1;
    static constexpr usize BITS = __CHAR_BIT__;
};

export template <>
struct Limits<char> {
    static constexpr char MIN = -__SCHAR_MAX__ - 1;
    static constexpr char MAX = __SCHAR_MAX__;
    static constexpr bool SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ - 1;
    static constexpr usize BITS = __CHAR_BIT__;
};

export template <>
struct Limits<short> {
    static constexpr short MIN = -__SHRT_MAX__ - 1;
    static constexpr short MAX = __SHRT_MAX__;
    static constexpr bool SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(short) - 1;
    static constexpr usize BITS = __CHAR_BIT__ * sizeof(short);
};

export template <>
struct Limits<int> {
    static constexpr int MIN = -__INT_MAX__ - 1;
    static constexpr int MAX = __INT_MAX__;
    static constexpr bool SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(int) - 1;
    static constexpr usize BITS = __CHAR_BIT__ * sizeof(int);
};

export template <>
struct Limits<long> {
    static constexpr long MIN = -__LONG_MAX__ - 1;
    static constexpr long MAX = __LONG_MAX__;
    static constexpr bool SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(long) - 1;
    static constexpr usize BITS = __CHAR_BIT__ * sizeof(long);
};

export template <>
struct Limits<long long> {
    static constexpr long long MIN = -__LONG_LONG_MAX__ - 1;
    static constexpr long long MAX = __LONG_LONG_MAX__;
    static constexpr bool SIGNED = true;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(long long) - 1;
    static constexpr usize BITS = __CHAR_BIT__ * sizeof(long long);
};

export template <>
struct Limits<unsigned char> {
    static constexpr unsigned char MIN = 0;
    static constexpr unsigned char MAX = __SCHAR_MAX__ * 2u + 1;
    static constexpr bool SIGNED = false;
    static constexpr usize DIGITS = __CHAR_BIT__;
    static constexpr usize BITS = __CHAR_BIT__;
};

export template <>
struct Limits<unsigned short> {
    static constexpr unsigned short MIN = 0;
    static constexpr unsigned short MAX = __SHRT_MAX__ * 2u + 1;
    static constexpr bool SIGNED = false;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(short);
    static constexpr usize BITS = __CHAR_BIT__ * sizeof(short);
};

export template <>
struct Limits<unsigned> {
    static constexpr unsigned MIN = 0;
    static constexpr unsigned MAX = __INT_MAX__ * 2u + 1;
    static constexpr bool SIGNED = false;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(int);
    static constexpr usize BITS = __CHAR_BIT__ * sizeof(int);
};

export template <>
struct Limits<unsigned long> {
    static constexpr unsigned long MIN = 0;
    static constexpr unsigned long MAX = __LONG_MAX__ * 2ul + 1;
    static constexpr bool SIGNED = false;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(long);
    static constexpr usize BITS = __CHAR_BIT__ * sizeof(long);
};

export template <>
struct Limits<unsigned long long> {
    static constexpr unsigned long long MIN = 0;
    static constexpr unsigned long long MAX = __LONG_LONG_MAX__ * 2ull + 1;
    static constexpr bool SIGNED = false;
    static constexpr usize DIGITS = __CHAR_BIT__ * sizeof(long long);
    static constexpr usize BITS = __CHAR_BIT__ * sizeof(long long);
};

#ifndef __ck_freestanding__
export template <>
struct Limits<float> {
    static constexpr float MIN = -__FLT_MAX__;
    static constexpr float SMALLEST_NORMAL = __FLT_MIN__;
    static constexpr float SMALLEST_DENORMAL = __FLT_DENORM_MIN__;
    static constexpr float MAX = __FLT_MAX__;
    static constexpr float EPSILON = __FLT_EPSILON__;
    static constexpr bool SIGNED = true;
    static constexpr usize DIGITS = __FLT_MANT_DIG__;
};

export template <>
struct Limits<double> {
    static constexpr double MIN = -__DBL_MAX__;
    static constexpr double SMALLEST_NORMAL = __DBL_MIN__;
    static constexpr double SMALLEST_DENORMAL = __DBL_DENORM_MIN__;
    static constexpr double MAX = __DBL_MAX__;
    static constexpr double EPSILON = __DBL_EPSILON__;
    static constexpr bool SIGNED = true;
    static constexpr usize DIGITS = __DBL_MANT_DIG__;
};

export template <>
struct Limits<long double> {
    static constexpr long double MIN = -__LDBL_MAX__;
    static constexpr long double SMALLEST_NORMAL = __LDBL_MIN__;
    static constexpr long double SMALLEST_DENORMAL = __LDBL_DENORM_MIN__;
    static constexpr long double MAX = __LDBL_MAX__;
    static constexpr long double EPSILON = __LDBL_EPSILON__;
    static constexpr bool SIGNED = true;
    static constexpr usize DIGITS = __LDBL_MANT_DIG__;
};
#endif

} // namespace Karm
