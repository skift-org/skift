#pragma once

#include <libfile/Archive.h>

class ZipArchive : public Archive
{
public:
    ZipArchive(File file);

    Result extract(unsigned int entry_index, const char *dest_path) override;
    Result insert(const char *entry_name, const char *src_path) override;
};