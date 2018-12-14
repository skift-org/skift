#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

int atapio_read (u8 drive, u32 numblock, u8 count, char *buf);
int atapio_write(u8 drive, u32 numblock, u8 count, char *buf);