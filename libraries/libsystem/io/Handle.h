#pragma once

#include <abi/Handle.h>

#define handle_printf_error(__handle, __args...) __handle_printf_error(HANDLE(__handle), __args)

int __handle_printf_error(Handle *handle, const char *fmt, ...);
