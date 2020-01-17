#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "multiboot.h"

#include <libsystem/runtime.h>

bool framebuffer_initialize(multiboot_info_t *mboot);

void keyboard_initialize(void);

void null_initialize(void);

void proc_initialize(void);

void random_initialize(void);

void textmode_initialize(void);

void zero_initialize(void);

void serial_initialize(void);

void mouse_initialize(void);