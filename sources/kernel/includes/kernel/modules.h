#pragma once
#include "types.h"
#include "kernel/multiboot.h"

void modules_setup(multiboot_info_t *minfo);
uint modules_get_end(multiboot_info_t *minfo);

void ramdisk_load(multiboot_module_t *module);