#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

#include "multiboot.h"

void modules_setup(multiboot_info_t *minfo);
uint modules_get_end(multiboot_info_t *minfo);

void ramdiload(multiboot_module_t *module);