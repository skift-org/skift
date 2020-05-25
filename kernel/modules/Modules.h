#pragma once

#include "kernel/multiboot/Multiboot.h"

void modules_initialize(Multiboot *multiboot);

void ramdisk_load(Module *module);
