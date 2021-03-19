#pragma once

#include "kernel/handover/Handover.h"

void graphic_early_initialize(Handover *handover);

void graphic_initialize(Handover *handover);

void framebuffer_initialize(Handover *handover);

bool graphic_has_framebuffer();

void graphic_did_find_framebuffer(uintptr_t address, int width, int height);

uint32_t *graphic_framebuffer();

int graphic_framebuffer_width();

int graphic_framebuffer_height();
