/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include <string.h>

#include <skift/__plugs__.h>

int puts(const char * str)
{
    __plug_print(str);
    return strlen(str);
}
