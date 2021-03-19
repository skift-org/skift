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
    void read_archive();
    Result read_central_directory(IO::Reader &reader);

    void write_archive();
    void write_entry(const Entry &entry, IO::Writer &writer, IO::Reader &compressed_data);
};