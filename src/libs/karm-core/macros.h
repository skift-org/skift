#pragma once

#define lifetimebound [[clang::lifetimebound]]
#define __concat$(LHS, RHS) LHS##RHS
#define concat$(LHS, RHS) __concat$(LHS, RHS)
#define __stringify$(SYM) #SYM
#define stringify$(SYM) __stringify$(SYM)
#define var$(NAME) concat$(__m_##NAME##_, __LINE__)
#define always_inline [[gnu::always_inline]]

#ifdef _MSC_VER
#    define no_unique_address msvc::no_unique_address
#else
#    define no_unique_address no_unique_address
#endif

// MARK: Try -------------------------------------------------------------------

#define __try$(EXPR, RET, AWAIT, FINAL) ({            \
    auto __expr = AWAIT(EXPR);                        \
    if (not static_cast<bool>(__expr)) [[unlikely]] { \
        RET __expr.none();                            \
        FINAL;                                        \
    }                                                 \
    __expr.take();                                    \
})

// clang-format off
#define try$(EXPR)      __try$(EXPR,    return,          ,          )
#define co_try$(EXPR)   __try$(EXPR, co_return,          ,          )
#define co_trya$(EXPR)  __try$(EXPR, co_return,  co_await,          )
#define co_tryy$(EXPR)  __try$(EXPR,  co_yield,          , co_return)
#define co_tryya$(EXPR) __try$(EXPR,  co_yield,  co_await, co_return)
// clang-format on
