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

class TARArchive : public Archive
{
private:
    void read_archive();

public:
    TARArchive(Path path, bool read = true);

    Result extract(unsigned int entry_index, const char *dest_path) override;
    Result insert(const char *entry_name, const char *src_path) override;
};