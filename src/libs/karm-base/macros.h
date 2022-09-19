#pragma once

namespace Karm {

#define arrayLen$(ARR) (sizeof(ARR) / sizeof(ARR[0]))

#define __concat$(LHS, RHS) LHS##RHS

#define concat$(LHS, RHS) __concat$(LHS, RHS)

#define __stringify$(SYM) #SYM

#define stringify$(SYM) __stringify$(SYM)

#define var$(NAME) concat$(NAME, __LINE__)

#define defer$(BEGIN, END) for (int var$(__i) = (BEGIN, 0); !var$(__i); (var$(__i) += 1, END))

#define condDefer$(BEGIN, END) for (int var$(__i) = BEGIN; var$(__i); (var$(__i) -= 1, END))

#define ALWAYS_INLINE __attribute__((always_inline))

} // namespace Karm
