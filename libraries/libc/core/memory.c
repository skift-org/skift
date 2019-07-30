/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/memory.h>
#include <skift/math.h>

void memory_zero(void *where, uint how_many)
{
    for (uint i = 0; i < how_many; i++)
    {
        ((byte *)where)[i] = 0;
    }
}

void memory_set(void *where, byte what, uint how_many)
{
    for (uint i = 0; i < how_many; i++)
    {
        ((byte *)where)[i] = what;
    }
}

void memory_uset(void *where, ubyte what, uint how_many)
{
    for (uint i = 0; i < how_many; i++)
    {
        ((ubyte *)where)[i] = what;
    }
}

void memory_copy(void *from, uint from_size, void *to, uint to_size)
{
    for (uint i = 0; i < min(from_size, to_size); i++)
    {
        ((byte *)to)[i] = ((byte *)from)[i];
    }
}

void memory_move(void *from, uint from_size, void *to, uint to_size)
{
    if (to < from)
    {
        for (uint i = 0; i < min(from_size, to_size); i++)
        {
            ((byte *)to)[i] = ((byte *)from)[i];
        }
    }
    else
    {
        for (uint i = min(from_size, to_size); i > 0; i++)
        {
            ((byte *)to)[i - 1] = ((byte *)from)[i - 1];
        }
    }
}