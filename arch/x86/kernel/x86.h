#pragma once

#include "arch/Arch.h"

static inline void cli() { asm volatile("cli"); }

static inline void sti() { asm volatile("sti"); }

static inline void hlt() { asm volatile("hlt"); }
