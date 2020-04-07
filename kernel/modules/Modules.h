#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/multiboot/Multiboot.h"

void modules_initialize(Multiboot *multiboot);

void ramdisk_load(Module *module);
