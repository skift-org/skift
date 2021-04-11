#pragma once

#include <libsystem/unicode/UnicodeString.h>

void history_commit(UnicodeString *text);

UnicodeString *history_peek(size_t index);

size_t history_length();
