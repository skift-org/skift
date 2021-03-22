#pragma once

#include <libfile/Archive.h>
#include <libio/Reader.h>
#include <libio/Writer.h>

class ZipArchive : public Archive
{
public:
    ZipArchive(Path path, bool read = true);

    Result extract(unsigned int entry_index, const char *dest_path) override;
    Result insert(const char *entry_name, const char *src_path) override;

private:
    Result read_archive();
    void write_archive();
};