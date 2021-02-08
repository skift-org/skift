#pragma once

#include <libsystem/io_new/Reader.h>
#include <libsystem/io_new/Seek.h>
#include <libsystem/io_new/Writer.h>

namespace System
{

template <typename T>
concept Duplex = IsBaseOf<Reader, T>::value &&IsBaseOf<Writer, T>::value;

template <typename T>
concept SeekableDuplex = Duplex<T> &&IsBaseOf<Seek, T>::value;

} // namespace System