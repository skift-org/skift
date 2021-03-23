#pragma once

#include <libtest/Asserts.h>

namespace Test
{

typedef void (*TestFunction)();

struct Test;

void __register_test(Test &test);

struct Test
{
    const char *name;
    TestFunction function;
    __SOURCE_LOCATION__ location;

    Test(const char *name, TestFunction function, __SOURCE_LOCATION__ location)
    {
        this->name = name;
        this->function = function;
        this->location = location;

        __register_test(*this);
    }
};

#define TEST(__test_function)                  \
    void __##__test_function##_function();     \
    ::Test::Test __##__test_function##_object{ \
        #__test_function,                      \
        __##__test_function##_function,        \
        SOURCE_LOCATION,                       \
    };                                         \
    void __##__test_function##_function()

int run_all_testes();

} // namespace Test