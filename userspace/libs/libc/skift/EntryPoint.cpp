#include <libc/cxx/cxx.h>
#include <skift/Environment.h>
#include <stdint.h>
#include <stdlib.h>

extern "C" int main(int argc, char **argv);

extern "C" void _init();
extern "C" void _fini();

static void __initialize()
{
    _init();

    extern void (*__init_array_start[])(int, char **, char **) __attribute__((visibility("hidden")));
    extern void (*__init_array_end[])(int, char **, char **) __attribute__((visibility("hidden")));

    const size_t size = __init_array_end - __init_array_start;
    for (size_t i = 0; i < size; i++)
    {
        (*__init_array_start[i])(0, nullptr, nullptr);
    }
}

static void __uninitialize(int exit_code)
{
    _fini();
    __cxa_finalize(nullptr);

    exit(exit_code);
}

extern "C" void __entry_point(int argc, char **argv, char *env)
{
    __initialize();

    environment_load(env);

    int exit_value = main(argc, argv);

    __uninitialize(exit_value);
}