#pragma once

#include <libutils/Assert.h>
#include <libutils/SourceLocation.h>

namespace Test
{

typedef void (*TestFunction)();

struct Test;

void __register_test(Test &test);

enum Flags
{
    NONE = 0,
    EXPECTED_TO_FAIL = 1 << 0,
};

struct Test
{

    Flags flags;
    const char *name;
    TestFunction function;
    Utils::SourceLocation location;

    Test(Flags flags, const char *name, TestFunction function, Utils::SourceLocation location = Utils::SourceLocation::current())
    {
        this->flags = flags;
        this->name = name;
        this->function = function;
        this->location = location;

        __register_test(*this);
    }
};

#define TEST(__test_function) TESTF(__test_function, ::Test::NONE)

#define TESTF(__test_function, __flags)             \
    void __test_##__test_function##_function();     \
    ::Test::Test __test_##__test_function##_object{ \
        (__flags),                                  \
        #__test_function,                           \
        __test_##__test_function##_function,        \
    };                                              \
    void __test_##__test_function##_function()

int run_all_testes();

} // namespace Test