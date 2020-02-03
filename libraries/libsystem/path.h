#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Filesystem.h>

#include <libsystem/list.h>
#include <libsystem/runtime.h>

typedef struct
{
    List *elements;
    bool is_absolue;
} Path;

Path *path(const char *raw_path);

void path_delete(Path *p);

const char *path_filename(Path *p);

const char *path_element(Path *p, int index);

bool path_is_absolue(Path *p);

bool path_is_relative(Path *p);

int path_length(Path *p);

void path_normalize(Path *p);

void path_push(Path *p, const char *element);

const char *path_pop(Path *p);

// Combine two path into a new one.
Path *path_combine(Path *left, Path *right);

// Split the path a the index (inclusif)
Path *path_split_at(Path *path, int index);

// Duplicate a path
Path *path_dup(Path *path);

void Patho_cstring(Path *this, char *buffer, uint size);

void path_dump(Path *p);