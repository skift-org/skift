#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#include <thirdparty/multiboot/Multiboot.h>

void modules_setup(multiboot_info_t *minfo);
uint modules_get_end(multiboot_info_t *minfo);
void ramdisk_load(multiboot_module_t *module);
