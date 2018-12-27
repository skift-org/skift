#pragma once

void sk_atomic_enable();
void sk_atomic_disable();
void sk_atomic_begin();
void sk_atomic_end();

#define ATOMIC(code)       \
    do                     \
    {                      \
        sk_atomic_begin(); \
        code;              \
        sk_atomic_end();   \
    } while (0);
