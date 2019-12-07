#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "multiboot.h"

void framebuffer_setup(multiboot_info_t *mboot);

void keyboard_setup(void);

void null_setup(void);

void proc_setup(void);

void random_setup(void);

void textmode_setup(void);

void zero_setup(void);
