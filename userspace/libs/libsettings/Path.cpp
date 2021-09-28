#include <libio/Format.h>
#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>
#include <libio/Write.h>
#include <libsettings/Path.h>

namespace Settings
{

Path Path::parse(IO::Scanner &scan)
{
    Path path;

    auto parse_string = [&](char sep) {
        IO::MemoryWriter memory;

        while (scan.peek() != sep && !scan.ended())
        {
            IO::write(memory, scan.next());
        }

        return memory.string();
    };

    path.domain = parse_string(':');

    if (!scan.skip(':'))
    {
        return path;
    }

    path.bundle = parse_string('.');

    if (!scan.skip('.'))
    {
        return path;
    }

    path.key = parse_string('\0');

    return path;
}

Path Path::parse(String str)
{
    IO::MemoryReader memory{str};
    IO::Scanner scan{memory};
    return parse(scan);
};

Path Path::parse(const char *str, size_t size)
{
    IO::MemoryReader memory{str, size};
    IO::Scanner scan{memory};
    return parse(scan);
};

void Path::prettify(IO::Prettier &pretty) const
{
    IO::format(pretty, "{}:{}.{}", domain, bundle, key);
}

bool Path::match(const Path &other) const
{
    return (other.domain == "*" || other.domain == domain) &&
           (other.bundle == "*" || other.bundle == bundle) &&
           (other.key == "*" || other.key == key);
}

bool Path::operator==(const Path &other)
{
    return domain == other.domain &&
           bundle == other.domain &&
           key == other.key;
}

} // namespace Settings
