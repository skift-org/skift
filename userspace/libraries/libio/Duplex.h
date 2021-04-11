#pragma once

#include <libio/Reader.h>
#include <libio/Seek.h>
#include <libio/Writer.h>

namespace IO
{

template <typename T>
concept Duplex = IsBaseOf<Reader, T>::value &&IsBaseOf<Writer, T>::value;

template <typename T>
concept SeekableDuplex = Duplex<T> &&IsBaseOf<Seek, T>::value;

} // namespace IO