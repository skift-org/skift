#pragma once

namespace Karm {

#define lifetimebound [[clang::lifetimebound]]
#define __concat$(LHS, RHS) LHS##RHS
#define concat$(LHS, RHS) __concat$(LHS, RHS)
#define __stringify$(SYM) #SYM
#define stringify$(SYM) __stringify$(SYM)
#define var$(NAME) concat$(__m_##NAME##_, __LINE__)
#define always_inline [[gnu::always_inline]]

#ifdef _MSC_VER
#    define no_unique_address [[msvc::no_unique_address]]
#else
#    define no_unique_address [[no_unique_address]]
#endif

// MARK: Enum ------------------------------------------------------------------

#define FlagsEnum$(T)                              \
    inline T operator~(T a) {                      \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T) ~(U)a;                          \
    }                                              \
    inline T operator|(T a, T b) {                 \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T)((U)a | (U)b);                   \
    }                                              \
    inline T operator&(T a, T b) {                 \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T)((U)a & (U)b);                   \
    }                                              \
    inline T operator^(T a, T b) {                 \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T)((U)a ^ (U)b);                   \
    }                                              \
    inline bool operator!(T a) {                   \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return not(U) a;                           \
    }                                              \
    inline T& operator|=(T& a, T b) {              \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T&)((U&)a |= (U)b);                \
    }                                              \
    inline T& operator&=(T& a, T b) {              \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T&)((U&)a &= (U)b);                \
    }                                              \
    inline T& operator^=(T& a, T b) {              \
        using U = ::Karm::Meta::UnderlyingType<T>; \
        return (T&)((U&)a ^= (U)b);                \
    }

// MARK: Try -------------------------------------------------------------------

// Give us a symbole to break one when debbuging error handling.
// This is a no-op in release mode.
#if defined(__ck_debug__) and !defined(KARM_DISABLE_TRY_FAIL_HOOK)
extern "C" void __try_failed();
#    define __tryFail() __try_failed()
#else
#    define __tryFail() /* NOP */
#endif

#define __try$(EXPR, RET, AWAIT) ({                   \
    auto __expr = AWAIT(EXPR);                        \
    if (not static_cast<bool>(__expr)) [[unlikely]] { \
        __tryFail();                                  \
        RET __expr.none();                            \
    }                                                 \
    __expr.take();                                    \
})

#define try$(EXPR) __try$(EXPR, return, )

#define co_try$(EXPR) __try$(EXPR, co_return, )

#define co_trya$(EXPR) __try$(EXPR, co_return, co_await)

} // namespace Karm
