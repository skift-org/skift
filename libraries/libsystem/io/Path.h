#pragma once

#include <abi/Filesystem.h>

#include <libsystem/utils/List.h>

struct Path
{
    List *elements;
    bool is_absolute;
};

Path *path_create(const char *raw_path);

void path_destroy(Path *path);

const char *path_filename(Path *path);

const char *path_extension(Path *path);

const char *path_peek_at(Path *path, int index);

bool path_is_absolute(Path *path);

bool path_is_relative(Path *path);

size_t path_element_count(Path *path);

void path_normalize(Path *path);

void path_push(Path *path, const char *element);

char *path_pop(Path *path);

Path *path_combine(Path *left, Path *right);

Path *path_clone(Path *path);

void path_to_cstring(Path *path, char *buffer, uint size);

char *path_as_string(Path *path);

void path_dump(Path *path);

bool path_equals(Path *left, Path *right);
