#include <libc/cxx/cxx.h>
#include <libc/skift/Environment.h>
#include <stdint.h>
#include <stdlib.h>

extern "C" int main(int argc, char **argv);

extern "C" void _init();
extern "C" void _fini();

void initialize()
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

void uninitialize(int exit_code)
{
    _fini();
    __cxa_finalize(nullptr);

    exit(exit_code);
}

extern "C" void __entry_point(int argc, char **argv, char *env)
{
    initialize();

    __unused(env);
    environment_load(env);

    int exit_value = main(argc, argv);

    uninitialize(exit_value);
}