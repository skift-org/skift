#include <libsystem/Logger.h>
#include <libfile/ZipArchive.h>
#include <libutils/Endian.h>
#include <libutils/SliceReader.h>

#define ZIP_LOCAL_DIR_HEADER_SIG 0x04034b50
#define ZIP_LOCAL_DIR_HEADER_SIZE = 30

enum EntryFlags : uint16_t
{
    EF_ENCRYPTED            = 1 << 0,
    EF_COMPRESSION_OPTION   = 1 << 1,
    EF_COMPRESSION_OPTION2  = 1 << 2,
    EF_DATA_DESCRIPTOR      = 1 << 3,
};

using le_flags = LittleEndian<EntryFlags>;

enum CompressionMethod : uint16_t
{
    CM_UNCOMPRESSED = 0,
    CM_SHRUNK       = 1,
    CM_DEFLATED     = 8,
};

using le_compression = LittleEndian<CompressionMethod>;

struct __packed LocalHeader
{
    le_uint32_t     signature;
    le_uint16_t     version;
    le_flags        flags;
    le_compression  compression;
    le_uint16_t     mod_time;
    le_uint16_t     mod_date;
    le_uint32_t     crc;
    le_uint32_t     compressed_size;
    le_uint32_t     uncompressed_size;
    le_uint16_t     len_filename;
    le_uint16_t     len_extrafield;
};

static_assert(sizeof(LocalHeader)==30, "LocalHeader has invalid size!");

struct __packed DataDescriptor
{
    le_uint32_t crc;
    le_uint32_t compressed_size;
    le_uint32_t uncompressed_size;
};

static_assert(sizeof(DataDescriptor)==12, "DataDescriptor has invalid size!");

ZipArchive::ZipArchive(File file) : Archive(file)
{
    ResultOr<Slice> content = _file.read_all();

    if (!content.success())
        return;

    // TODO: read metadata from the zip archive header
    SliceReader reader(content.value());
    
    while(reader.good())
    {
        auto local_header = reader.get<LocalHeader>();
        printf("Signature: %i\n", local_header.signature());
        if(local_header.signature() != ZIP_LOCAL_DIR_HEADER_SIG)
            break;

        auto filename = reader.get_fixed_len_string(local_header.len_filename());
        reader.skip(local_header.len_extrafield());

        printf("Filename: %s\n", filename.cstring());
        printf("Flags: %i\n", local_header.flags());
        printf("Extrasize: %i\n", local_header.len_extrafield());

        uint32_t uncompressed_size = local_header.uncompressed_size();
        uint32_t compressed_size = local_header.compressed_size();

        printf("Uncompressed Size: %u\n", uncompressed_size);
        printf("Compressed Size: %u\n", compressed_size);

        reader.skip(compressed_size);

        if(local_header.flags() & EF_DATA_DESCRIPTOR)
        {
            auto data_descriptor = reader.get<DataDescriptor>();
            uncompressed_size = data_descriptor.uncompressed_size();
            compressed_size = data_descriptor.compressed_size();
        }

        printf("DD Uncompressed Size: %u\n", uncompressed_size);
        printf("DD Compressed Size: %u\n", compressed_size);

    } 

    _valid = true;
}

Archive::Entry ZipArchive::get_entry(unsigned int entry_index)
{
    __unused(entry_index);
    return Entry();
}

Result ZipArchive::extract(unsigned int entry_index, const char *dest_path)
{
    __unused(entry_index);
    __unused(dest_path);
    // TODO: use inflate to extract the compressed entry and write it to the dest_path
    return Result::ERR_FUNCTION_NOT_IMPLEMENTED;
}

Result ZipArchive::insert(const char *entry_name, const char *src_path)
{
    __unused(entry_name);
    __unused(src_path);
    // TODO: use deflate to compress the src_path and store it in entry_name
    return Result::ERR_FUNCTION_NOT_IMPLEMENTED;
}