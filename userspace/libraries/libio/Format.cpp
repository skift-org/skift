#include <libio/Format.h>
#include <libutils/NumberFormat.h>

namespace IO
{

ResultOr<size_t> format(Writer &writer, const Formating &formating, char value)
{
    if (formating.type == Formating::CHARACTER)
    {
        return IO::write(writer, value);
    }
    else
    {
        return NumberFormat::decimal().format(writer, (int64_t)value);
    }
}

ResultOr<size_t> format(Writer &writer, const Formating &formating, unsigned char value)
{
    if (formating.type == Formating::CHARACTER)
    {
        return IO::write(writer, value);
    }
    else
    {
        return NumberFormat::decimal().format(writer, (uint64_t)value);
    }
}

ResultOr<size_t> format(Writer &writer, const Formating &, short int value)
{
    return NumberFormat::decimal().format(writer, (int64_t)value);
}

ResultOr<size_t> format(Writer &writer, const Formating &, unsigned short int value)
{
    return NumberFormat::decimal().format(writer, (uint64_t)value);
}

ResultOr<size_t> format(Writer &writer, const Formating &, int value)
{
    return NumberFormat::decimal().format(writer, (int64_t)value);
}

ResultOr<size_t> format(Writer &writer, const Formating &, unsigned int value)
{
    return NumberFormat::decimal().format(writer, (uint64_t)value);
}

ResultOr<size_t> format(Writer &writer, const Formating &, long int value)
{
    return NumberFormat::decimal().format(writer, (int64_t)value);
}

ResultOr<size_t> format(Writer &writer, const Formating &, unsigned long int value)
{
    return NumberFormat::decimal().format(writer, (uint64_t)value);
}

ResultOr<size_t> format(IO::Writer &writer, const Formating &, long long int value)
{
    return NumberFormat::decimal().format(writer, (int64_t)value);
}

ResultOr<size_t> format(IO::Writer &writer, const Formating &, unsigned long long int value)
{
    return NumberFormat::decimal().format(writer, (uint64_t)value);
}

ResultOr<size_t> format(Writer &writer, const Formating &, float value)
{
    return NumberFormat::decimal().format(writer, value);
}

ResultOr<size_t> format(Writer &writer, const Formating &, double value)
{
    return NumberFormat::decimal().format(writer, value);
}

ResultOr<size_t> format(Writer &writer, const Formating &, const char *cstring)
{
    return IO::write(writer, cstring);
}

ResultOr<size_t> format(Writer &writer, const Formating &, const String string)
{
    return IO::write(writer, string.cstring());
}

ResultOr<size_t> format(Writer &writer, const Formating &, const IO::Path path)
{
    return IO::write(writer, path.string());
}

} // namespace IO
