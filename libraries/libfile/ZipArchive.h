#pragma once

#include <libfile/Archive.h>
#include <libutils/BinaryWriter.h>

class ZipArchive : public Archive
{
public:
    ZipArchive(File file, bool read = true);

    Result extract(unsigned int entry_index, const char *dest_path) override;
    Result insert(const char *entry_name, const char *src_path) override;

private:
    void get_compressed_data(const Entry &entry, Vector<uint8_t> &compressed_data);

    void read_archive();

    void write_archive();
    void write_entry(const Entry &entry, BinaryWriter &writer, const Vector<uint8_t> &compressed_data);
    void write_central_directory(BinaryWriter &writer);

    uint8_t *_data = nullptr;
    size_t _size = 0;
};