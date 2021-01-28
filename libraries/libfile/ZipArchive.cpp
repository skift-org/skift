#include <libfile/ZipArchive.h>
#include <libsystem/Logger.h>
#include <libsystem/compression/Inflate.h>
#include <libutils/Endian.h>
#include <libutils/SliceReader.h>

// Central header
#define ZIP_END_OF_CENTRAL_DIR_HEADER_SIG 0x06054b50
#define ZIP_CENTRAL_DIR_HEADER_SIG 0x02014b50

// Local header
#define ZIP_LOCAL_DIR_HEADER_SIG 0x04034b50

enum ExtraFieldType : uint16_t
{
    EFT_ZIP64 = 0x0001,
    EFT_EXTENDED_TIMESTAMP = 0x5455,
    EFT_NEW_UNIX = 0x7875,
};

using le_eft = LittleEndian<ExtraFieldType>;

enum EntryFlags : uint16_t
{
    EF_ENCRYPTED = 1 << 0,
    EF_COMPRESSION_OPTION = 1 << 1,
    EF_COMPRESSION_OPTION2 = 1 << 2,
    EF_DATA_DESCRIPTOR = 1 << 3,
};

using le_flags = LittleEndian<EntryFlags>;

enum CompressionMethod : uint16_t
{
    CM_UNCOMPRESSED = 0,
    CM_SHRUNK = 1,
    CM_DEFLATED = 8,
};

using le_compression = LittleEndian<CompressionMethod>;

struct __packed LocalHeader
{
    le_uint32_t signature;
    le_uint16_t version;
    le_flags flags;
    le_compression compression;
    le_uint16_t mod_time;
    le_uint16_t mod_date;
    le_uint32_t crc;
    le_uint32_t compressed_size;
    le_uint32_t uncompressed_size;
    le_uint16_t len_filename;
    le_uint16_t len_extrafield;
};

static_assert(sizeof(LocalHeader) == 30, "LocalHeader has invalid size!");

struct __packed DataDescriptor
{
    le_uint32_t crc;
    le_uint32_t compressed_size;
    le_uint32_t uncompressed_size;
};

static_assert(sizeof(DataDescriptor) == 12, "DataDescriptor has invalid size!");

ZipArchive::ZipArchive(File file) : Archive(file)
{
    ResultOr<Slice> content = _file.read_all();

    if (!content.success())
        return;

    SliceReader reader(content.value());

    // Read all local file headers and data descriptors
    while (reader.good())
    {
        auto local_header = reader.peek<LocalHeader>();

        // Check if this is a local header
        if (local_header.signature() != ZIP_LOCAL_DIR_HEADER_SIG)
        {
            break;
        }

        auto &entry = _entries.emplace_back();
        reader.skip(sizeof(LocalHeader));

        // Get the uncompressed & compressed sizes
        entry.uncompressed_size = local_header.uncompressed_size();
        entry.compressed_size = local_header.compressed_size();
        entry.compression = local_header.compression();

        // Read the filename of this entry
        entry.name = reader.get_fixed_len_string(local_header.len_filename());

        // Read extra fields
        auto end_index = reader.index() + local_header.len_extrafield();
        while (reader.index() < end_index)
        {
            le_eft extra_field_type(reader.get<ExtraFieldType>());
            le_uint16_t extra_field_size(reader.get<uint16_t>());

            // TODO: parse the known extra field types
            reader.skip(extra_field_size());
        }

        // Skip the compressed data for now
        entry.archive_offset = reader.index();
        reader.skip(entry.compressed_size);

        if (local_header.flags() & EF_DATA_DESCRIPTOR)
        {
            auto data_descriptor = reader.get<DataDescriptor>();
            entry.uncompressed_size = data_descriptor.uncompressed_size();
            entry.compressed_size = data_descriptor.compressed_size();
        }
    }

    // TODO: Read central header
    le_uint32_t central_header_sig(reader.get<uint32_t>());
    if (central_header_sig() != ZIP_CENTRAL_DIR_HEADER_SIG)
    {
        printf("Zip error: missing central header signature!");
        return;
    }

    //TODO: parse all the rest

    _valid = true;
}

Result ZipArchive::extract(unsigned int entry_index, const char *dest_path)
{
    // Read the compressed data from the entry
    const auto &entry = get_entry(entry_index);

    if (entry.compression != CM_DEFLATED)
    {
        printf("ZipArchive: Unsupported compression: %u\n", entry.compression);
        return Result::ERR_FUNCTION_NOT_IMPLEMENTED;
    }

    // TODO: This is probably super ineffective
    ResultOr<Slice> content = _file.read_all();

    if (!content.success())
    {
        return content.result();
    }

    Slice slice_compressed = content.value().slice(entry.archive_offset, entry.compressed_size);
    Inflate inf;
    Vector<uint8_t> compressed_data(ADOPT, (uint8_t *)slice_compressed.start(), slice_compressed.size());
    Vector<uint8_t> uncompressed_data;
    inf.perform(compressed_data, uncompressed_data);

    printf("Uncompressed size: %u\n", uncompressed_data.count());

    File dest_file(dest_path);
    dest_file.write_all(uncompressed_data.raw_storage(), uncompressed_data.count());

    // TODO: use inflate to extract the compressed entry and write it to the dest_path
    return Result::SUCCESS;
}

Result ZipArchive::insert(const char *entry_name, const char *src_path)
{
    __unused(entry_name);
    __unused(src_path);
    // TODO: use deflate to compress the src_path and store it in entry_name
    return Result::ERR_FUNCTION_NOT_IMPLEMENTED;
}