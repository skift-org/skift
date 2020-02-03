#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

typedef uint32_t Codepoint;

bool codepoint_is_digit(Codepoint codepoint);

bool codepoint_is_alpha(Codepoint codepoint);

int codepoint_numeric_value(Codepoint codepoint);

int codepoint_to_utf8(Codepoint codepoint, uint8_t *buffer);

char codepoint_to_cp437(Codepoint codepoint);
