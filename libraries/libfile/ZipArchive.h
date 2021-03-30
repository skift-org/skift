#pragma once

#include <libfile/Archive.h>

class ZipArchive : public Archive
{
public:
    ZipArchive(Path path, bool read = true);

    Result extract(unsigned int entry_index, IO::Writer &writer) override;
    Result insert(const char *entry_name, IO::Reader &reader) override;

private:
    Result read_archive();
    void write_archive();
};