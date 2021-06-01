#pragma once

#include <libfile/Archive.h>

struct ZipArchive : public Archive
{
public:
    ZipArchive(IO::Path path, bool read = true);

    HjResult extract(unsigned int entry_index, IO::Writer &writer) override;
    HjResult insert(const char *entry_name, IO::Reader &reader) override;

private:
    HjResult read_archive();
    void write_archive();
};