#pragma once

#include "kernel/multiboot/Multiboot.h"

void graphic_initialize(Multiboot *multiboot);

void framebuffer_initialize(Multiboot *multiboot);

void textmode_initialize(void);

void graphic_did_find_framebuffer(void);
