#pragma once

#define try$(EXPR)                 \
    ({                             \
        auto __expr = (EXPR);      \
        if (!__expr)               \
        {                          \
            return __expr.error(); \
        }                          \
        __expr.unwrap();           \
    })
