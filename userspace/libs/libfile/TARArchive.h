#pragma once

#include <libfile/Archive.h>

struct TARBlock
{
    char name[100];
    char typeflag;
    char linkname[100];
    size_t size;
    char *data;
};

bool tar_read(void *tarfile, TARBlock *block, size_t index);

struct TARArchive final : public Archive
{
private:
    HjResult read_archive();

public:
    TARArchive(IO::Path path, bool read = true);

    HjResult extract(unsigned int entry_index, IO::Writer &writer) override;
    HjResult insert(const char *entry_name, IO::Reader &reader) override;
};