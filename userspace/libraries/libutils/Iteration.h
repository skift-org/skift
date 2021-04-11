#pragma once

#include <libutils/Callback.h>

enum class Iteration
{
    CONTINUE,
    STOP,
};

template <typename T>
using IterationCallback = Callback<Iteration(T)>;
