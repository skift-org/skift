#include <libfile/ZipArchive.h>

ZipArchive::ZipArchive(File file) : Archive(file)
{
    ResultOr<Slice> content = _file.read_all();

    if (!content.success())
        return;

    // TODO: read metadata from the zip archive header
}

const char *ZipArchive::get_entry_name(unsigned int entry_index)
{
    return nullptr;
}

Result ZipArchive::extract(unsigned int entry_index, const char *dest_path)
{
    // TODO: use inflate to extract the compressed entry and write it to the dest_path
    return Result::ERR_FUNCTION_NOT_IMPLEMENTED;
}

Result ZipArchive::insert(const char *entry_name, const char *src_path)
{
    // TODO: use deflate to compress the src_path and store it in entry_name
    return Result::ERR_FUNCTION_NOT_IMPLEMENTED;
}