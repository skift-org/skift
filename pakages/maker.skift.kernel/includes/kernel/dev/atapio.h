#pragma once
#include <skift/types.h>

int atapio_read (u8 drive, u32 numblock, u8 count, char *buf);
int atapio_write(u8 drive, u32 numblock, u8 count, char *buf);