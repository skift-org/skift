#pragma once

#include <libsystem/Common.h>

typedef struct Vector Vector;

Vector *vector_create(size_t element_size, size_t preallocated);

void vector_destroy(Vector *vector);

void vector_capacity(Vector *vector, size_t capacity);

void *vector_element(Vector *vector, size_t index);

void vector_pushback(Vector *vector, void *element);

void vector_popback(Vector *vector, void *element);

bool vector_empty(Vector *vector);
