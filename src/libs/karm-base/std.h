#pragma once

// clang-format off

#include <coroutine>
#include <compare>
#include <initializer_list>
#include <new>
#include <utility>
#include <memory>

#include <ctype.h>
#include <limits.h>
#include <math.h>

#ifdef __ck_sys_darwin__
// Darwin is poluting the global namespace with unwanted macros.
#undef UNDERFLOW
#endif

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "bool.h"
#include "ints.h"
#include "keywords.h"

// clang-format on

#ifndef KARM_NO_TOP_LEVEL_USING

namespace Karm {
} // namespace Karm

using namespace Karm;

#endif
