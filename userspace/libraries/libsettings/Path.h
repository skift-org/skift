#pragma once

#include <libio/Prettier.h>
#include <libio/Scanner.h>

namespace Settings
{

struct Path
{
    String domain;
    String bundle;
    String key;

    static Path parse(IO::Scanner &scan);
    static Path parse(String str);
    static Path parse(const char *str, size_t size);

    void prettify(IO::Prettier &pretty) const;

    bool match(const Path &other) const;

    bool operator==(const Path &other);
};

} // namespace Settings
