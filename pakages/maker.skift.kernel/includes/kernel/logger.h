#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/generic.h>

#define log(x...) __log(__FUNCTION__, x)
void __log(const char * file, const char *message, ...);