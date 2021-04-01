#pragma once

#include <libinjection/Entity.h>
#include <libutils/Assert.h>

#define assert_same_entity(__a, __b)                                           \
    Assert::equal((uintptr_t) static_cast<Injection::Entity *>((__a).naked()), \
                  (uintptr_t) static_cast<Injection::Entity *>((__b).naked()))

#define assert_not_same_entity(__a, __b)                                           \
    Assert::not_equal((uintptr_t) static_cast<Injection::Entity *>((__a).naked()), \
                      (uintptr_t) static_cast<Injection::Entity *>((__b).naked()))
