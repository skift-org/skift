#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/types.h>

int path_len(const char *path);
int path_read(const char *path, int index, char *buffer);
char *path_cat(const char *pathA, const char *pathB, char *dest);
int path_split(const char *path, char *dir, char *file);