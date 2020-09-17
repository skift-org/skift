#pragma once

#include "kernel/handover/Handover.h"

void graphic_initialize(Handover *handover);

void framebuffer_initialize(Handover *handover);

void textmode_initialize();

void graphic_did_find_framebuffer();
