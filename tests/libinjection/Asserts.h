#pragma once

#include <libinjection/Entity.h>
#include <libtest/Asserts.h>

#define assert_same_entity(__a, __b)                                          \
    assert_equal((uintptr_t) static_cast<Injection::Entity *>((__a).naked()), \
                 (uintptr_t) static_cast<Injection::Entity *>((__b).naked()))

#define assert_not_same_entity(__a, __b)                                          \
    assert_not_equal((uintptr_t) static_cast<Injection::Entity *>((__a).naked()), \
                     (uintptr_t) static_cast<Injection::Entity *>((__b).naked()))
