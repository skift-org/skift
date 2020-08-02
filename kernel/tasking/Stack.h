#pragma once

#include <libsystem/Common.h>

struct Stack
{
    void *buffer;
    uintptr_t pointer;
} ;

Stack *stack_create(size_t size);

void stack_destroy(Stack *stack);

void stack_push(Stack *stack, void *buffer, size_t data);

void stack_set_ptr(Stack *stack, uintptr_t pointer);

void stack_get_ptr(Stack *stack, uintptr_t pointer);
