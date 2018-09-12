#pragma once
#include "types.h"

void graphic_setup(uint width, uint height);
void graphic_blit(uint *buffer);

int graphic_is_textmode();
int graphic_is_bitmap();

void graphic_size_char(uint *width, uint *height);
void graphic_size_pixel(uint *width, uint *height);

void graphic_char(uint x, uint y, uchar c);
void graphic_pixel(uint x, uint y, uint color);