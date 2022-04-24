#pragma once

#include <karm-meta/value.h>

namespace Karm::Meta
{

template <typename T, typename U>
struct _Same : False
{
};

template <typename T>
struct _Same<T, T> : True
{
};

template <typename T, typename U>
concept Same = _Same<T, U>::VALUE;


} // namespace Karm::Meta
