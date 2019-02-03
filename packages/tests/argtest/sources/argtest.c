/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>

int main(int argc, char **argv)
{
    printf("argc=%d\n", argc);
    for (int i=0; i < argc; i++)
    {
        printf("argv[%d]='%s'\n", i+1, argv[i]);
    }

    return 0;
}
