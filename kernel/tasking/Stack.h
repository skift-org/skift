#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

typedef struct
{
    void *buffer;
    uintptr_t pointer;
} Stack;

Stack *stack_create(size_t size);

void stack_destroy(Stack *stack);

void stack_push(Stack *stack, void *buffer, size_t data);

void stack_set_ptr(Stack *stack, uintptr_t pointer);

void stack_get_ptr(Stack *stack, uintptr_t pointer);