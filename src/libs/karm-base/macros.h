#pragma once

namespace Karm {

#define goto _Pragma("GCC error \"goto is not allowed\"") goto

#define arrayLen$(ARR) (sizeof(ARR) / sizeof(ARR[0]))

#define __concat$(LHS, RHS) LHS##RHS

#define concat$(LHS, RHS) __concat$(LHS, RHS)

#define __stringify$(SYM) #SYM

#define stringify$(SYM) __stringify$(SYM)

#define var$(NAME) concat$(_MACRO_VAR_##NAME##__, __LINE__)

#define ALWAYS_INLINE [[gnu::always_inline]]

#define _ var$(placeholder)

/* --- Count ---------------------------------------------------------------- */

#define __count(_0, _1_, _2_, _3_, _4_, _5_, _6_, _7_, _8_, _9_, _10_, _11_, _12_, _13_, _14_, _15_, _16_, _17_, _18_, _19_, _20_, _21_, _22_, _23_, _24_, _25_, _26_, _27_, _28_, _29_, _30_, _31_, _32_, _33_, _34_, _35_, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, count, ...) count
#define count$(__args...) __count(0, ##__args, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/* --- Eval ----------------------------------------------------------------- */

#define __eval0(...) __VA_ARGS__
#define __eval1(...) __eval0(__eval0(__eval0(__VA_ARGS__)))
#define __eval2(...) __eval1(__eval1(__eval1(__VA_ARGS__)))
#define __eval3(...) __eval2(__eval2(__eval2(__VA_ARGS__)))
#define __eval4(...) __eval3(__eval3(__eval3(__VA_ARGS__)))

#define eval$(...) __eval4(__eval4(__eval4(__VA_ARGS__)))

/* --- Map ------------------------------------------------------------------ */

#define __MAP_END__(...)
#define __MAP_OUT__
#define __MAP_COMMA__ ,

#define __mapGetEnd2() 0, __MAP_END__
#define __mapGetEnd1(...) __mapGetEnd2
#define __mapGetEnd(...) __mapGetEnd1

#define __mapNext0(test, next, ...) next __MAP_OUT__
#define __mapNext1(test, next) __mapNext0(test, next, 0)
#define __mapNext(test, next) __mapNext1(__mapGetEnd test, next)

#define __map0(f, x, peek, ...) f(x) __mapNext(peek, __map1)(f, peek, __VA_ARGS__)
#define __map1(f, x, peek, ...) f(x) __mapNext(peek, __map0)(f, peek, __VA_ARGS__)

#define __mapListNext1(test, next) __mapNext0(test, __MAP_COMMA__ next, 0)
#define __mapListNext(test, next) __mapListNext1(__mapGetEnd test, next)

#define __mapList0(f, x, peek, ...) f(x) __mapListNext(peek, __mapList1)(f, peek, __VA_ARGS__)
#define __mapList1(f, x, peek, ...) f(x) __mapListNext(peek, __mapList0)(f, peek, __VA_ARGS__)

// Applies the function macro `f` to each of the remaining parameters.
#define map$(f, ...) eval$(__map1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

// Applies the function macro `f` to each of the remaining parameters and
// inserts commas between the results.
#define mapList$(f, ...) eval$(__mapList1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

/* --- Utilities ------------------------------------------------------------ */

template <typename T, typename U>
ALWAYS_INLINE static inline T unionCast(U value)
    requires(sizeof(T) == sizeof(U))
{
    union X {
        U u;
        T t;
    };

    return X{.u = value}.t;
}

} // namespace Karm
