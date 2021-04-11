#pragma once

#include <libsystem/Common.h>

void NO_RETURN assert_failed(const char *expr, const char *file, const char *function, int line);

#define ASSERT_NOT_REACHED() \
    assert("ASSERT_NOT_REACHED() reached!" && false)

#include_next <assert.h>
