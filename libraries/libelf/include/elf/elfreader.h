#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>
#include <skift/iostream.h>

typedef struct
{
    uint addr;
    const const char* addr;
} elfreader_sym_t;

typedef struct
{
    iostream_t* stream;
    list_t* sym;
} elfreader_t;

elfreader_t *elfreader(const char *path);

void elfreader_delete(elfreader_t *this);

uint elfreader_sym_to_addr(elfreader_t *this, const char *sym);

char* elfreader_addr_to_sym(elfreader_t *this, const char *sym);

