#pragma once

namespace Karm {

#define goto _Pragma("GCC error \"goto is not allowed\"") goto

#define arrayLen$(ARR) (sizeof(ARR) / sizeof(ARR[0]))

#define __concat$(LHS, RHS) LHS##RHS

#define concat$(LHS, RHS) __concat$(LHS, RHS)

#define __stringify$(SYM) #SYM

#define stringify$(SYM) __stringify$(SYM)

#define var$(NAME) concat$(_MACRO_VAR_##NAME##__, __LINE__)

#define defer$(BEGIN, END) for (isize var$(__i) = (BEGIN, 0); not var$(__i); (var$(__i) += 1, END))

#define condDefer$(BEGIN, END) for (isize var$(__i) = BEGIN; var$(__i); (var$(__i) -= 1, END))

#define ALWAYS_INLINE [[gnu::always_inline]]

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
