#include <stdlib.h>
#include <skift/__plugs__.h>

__attribute__((noreturn)) void abort(void)
{
    __plug_fini(-1);
}