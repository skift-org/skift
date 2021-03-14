#include <libutils/ArgParse.h>

#include "tests/Driver.h"

int main(int argc, char const *argv[])
{
    __unused(argc);
    __unused(argv);

    return Test::run_all_testes();
}
