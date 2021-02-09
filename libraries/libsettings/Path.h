#pragma once

#include <libutils/Prettifier.h>
#include <libutils/ScannerUtils.h>

namespace Settings
{

struct Path
{
    String domain;
    String bundle;
    String key;

    static Path parse(Scanner &scan);
    static Path parse(const String &str);
    static Path parse(const char *str, size_t size);

    void prettify(Prettifier &pretty) const;

    bool match(const Path &other) const;

    bool operator==(const Path &other);
};

} // namespace Settings
