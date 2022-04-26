#pragma once

#include "_prelude.h"

#define try$(EXPR)                 \
    ({                             \
        auto __expr = (EXPR);      \
        if (!__expr) {             \
            return __expr.error(); \
        }                          \
        __expr.unwrap();           \
    })
