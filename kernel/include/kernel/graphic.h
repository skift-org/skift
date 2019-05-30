#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>
#include "kernel/multiboot.h"

void graphic_early_setup(multiboot_info_t* mbootinfo);
void graphic_setup(void);