#pragma once

#include <libsystem/Common.h>

int early_console_width();

int early_console_height();

void early_console_disable();

void early_console_enable();

size_t early_console_write(const void *buffer, size_t size);
