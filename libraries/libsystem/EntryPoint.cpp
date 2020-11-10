#ifndef __KERNEL__

#include <libsystem/core/Plugs.h>
#include <libsystem/process/Environment.h>

extern "C" int main(int argc, char **argv);

extern "C" void __entry_point(int argc, char **argv, char *env)
{
    __plug_init();
    environment_load(env);
    int exit_value = main(argc, argv);
    __plug_fini(exit_value);
}

#endif
