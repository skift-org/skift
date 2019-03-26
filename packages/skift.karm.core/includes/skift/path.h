#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>
#include <skift/list.h>

typedef struct
{
    bool is_absolue;
    list_t* elements;
} path_t;

path_t* path(const char* raw_path);
void path_delete(path_t* p);

const char* path_filename(path_t* p);
const char* path_element(path_t* p, int index);
bool path_is_absolue(path_t* p);
bool path_is_relative(path_t* p);

int path_length(path_t* p);
void path_normalize(path_t* p);
void path_append(path_t* p, const char* element);
path_t* path_combine(path_t* left, path_t* right);

// old path manipulation api
int path_len(const char *path);
int path_read(const char *path, int index, char *buffer);
char *path_cat(const char *pathA, const char *pathB, char *dest);
int path_split(const char *path, char *dir, char *file);