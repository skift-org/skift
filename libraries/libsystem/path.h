#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <abi/Filesystem.h>

#include <libsystem/utils/List.h>

typedef struct
{
    List *elements;
    bool is_absolue;
} Path;

Path *path_create(const char *raw_path);

void path_destroy(Path *path);

const char *path_filename(Path *path);

const char *path_peek_at(Path *path, int index);

bool path_is_absolue(Path *path);

bool path_is_relative(Path *path);

size_t path_lenght(Path *path);

size_t path_element_count(Path *path);

void path_normalize(Path *path);

void path_push(Path *path, const char *element);

const char *path_pop(Path *path);

Path *path_combine(Path *left, Path *right);

Path *path_clone(Path *path);

void path_to_cstring(Path *path, char *buffer, uint size);

void path_dump(Path *path);
