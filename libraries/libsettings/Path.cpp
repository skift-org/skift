#include <libutils/ScannerUtils.h>

#include <libsettings/Path.h>

namespace Settings
{

Path Path::parse(Scanner &scan)
{
    Path path;

    auto parse_string = [&](char sep) {
        StringBuilder build;

        while (scan.current() != sep && scan.do_continue())
        {
            build.append(scan.current());
            scan.foreward();
        }

        return build.finalize();
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

Path Path::parse(const String &str)
{
    StringScanner scan{str.cstring(), str.length()};
    return parse(scan);
};

Path Path::parse(const char *str, size_t size)
{
    StringScanner scan{str, size};
    return parse(scan);
};

void Path::prettify(Prettifier &pretty) const
{
    pretty.append(domain);
    pretty.append(':');
    pretty.append(bundle);
    pretty.append('.');
    pretty.append(key);
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
