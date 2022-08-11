#pragma once

#include <new>
#include <utility>
#include <initializer_list>

#include "_prelude.h"

#ifdef __osdk_freestanding__

#    include <karm-meta/signess.h>

using ssize_t = Karm::Meta::MakeSigned<size_t>;

#endif
