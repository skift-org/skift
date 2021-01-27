#pragma once

#include <libfile/Archive.h>

class ZipArchive : public Archive
{
public:
    ZipArchive(File file);

    inline unsigned int get_entry_count() override
    {
        return _entry_count;
    }

    Entry get_entry(unsigned int entry_index) override;

    Result extract(unsigned int entry_index, const char *dest_path) override;
    Result insert(const char *entry_name, const char *src_path) override;

private:
    unsigned int _entry_count = 0;
};