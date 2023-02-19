#pragma once

// clang-format off

#include <new>
#include <utility>
#include <initializer_list>

#include "_prelude.h"
#include "bool.h"

// clang-format on

#ifndef __ssize_t_defined

#    include <karm-meta/signess.h>

using ssize_t = Karm::Meta::MakeSigned<size_t>;

#endif
