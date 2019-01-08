#include <stdio.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("\033[H\033[J");

    return 0;
}
