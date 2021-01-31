#include <libfile/ZipArchive.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/compression/Deflate.h>
#include <libsystem/compression/Inflate.h>
#include <libutils/BinaryReader.h>
#include <libutils/Endian.h>
#include <libutils/FileReader.h>
#include <libutils/FileWriter.h>
#include <libutils/MemoryReader.h>
#include <libutils/MemoryWriter.h>

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
    EF_NONE = 0,
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

struct __packed CentralDirectoryFileHeader
{
    le_uint32_t signature;
    le_uint16_t version;
    le_uint16_t version_required;
    le_flags flags;
    le_compression compression;
    le_uint16_t mod_time;
    le_uint16_t mod_date;
    le_uint32_t crc;
    le_uint32_t compressed_size;
    le_uint32_t uncompressed_size;
    le_uint16_t len_filename;
    le_uint16_t len_extrafield;
    le_uint16_t len_comment;
    le_uint16_t disk_start;
    le_uint16_t internal_attr;
    le_uint32_t external_attr;
    le_uint32_t local_header_offset;
};

struct __packed CentralDirectoryEndRecord
{
    le_uint32_t signature;
    le_uint16_t disk1;
    le_uint16_t disk2;
    le_uint16_t disk_entries;
    le_uint16_t total_entries;
    le_uint32_t central_dir_size;
    le_uint32_t central_dir_offset;
    le_uint16_t len_comment;
};

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

ZipArchive::ZipArchive(Path path, bool read) : Archive(path)
{
    if (read)
    {
        read_archive();
    }
}

void ZipArchive::read_archive()
{
    _valid = false;

    File archive_file = File(_path);

    // Archive does not exist
    if (!archive_file.exist())
    {
        printf("Archive does not exist: %s", _path.string().cstring());
        return;
    }

    FileReader file_reader(_path);

    // A valid zip must atleast contain a "CentralDirectoryEndRecord"
    if (file_reader.length() < sizeof(CentralDirectoryEndRecord))
    {
        printf("Archive is too small to be a valid .zip: %s %u", _path.string().cstring(), file_reader.length());
        return;
    }

    BinaryReader binary_reader(file_reader);

    // Read all local file headers and data descriptors
    while (binary_reader.good())
    {
        auto local_header = binary_reader.peek<LocalHeader>();

        // Check if this is a local header
        if (local_header.signature() != ZIP_LOCAL_DIR_HEADER_SIG)
        {
            break;
        }

        auto &entry = _entries.emplace_back();
        binary_reader.skip(sizeof(LocalHeader));

        // Get the uncompressed & compressed sizes
        entry.uncompressed_size = local_header.uncompressed_size();
        entry.compressed_size = local_header.compressed_size();
        entry.compression = local_header.compression();

        // Read the filename of this entry
        entry.name = binary_reader.get_fixed_len_string(local_header.len_filename());

        // Read extra fields
        auto end_position = binary_reader.position() + local_header.len_extrafield();
        while (binary_reader.position() < end_position)
        {
            le_eft extra_field_type(binary_reader.get<ExtraFieldType>());
            le_uint16_t extra_field_size(binary_reader.get<uint16_t>());

            // TODO: parse the known extra field types
            binary_reader.skip(extra_field_size());
        }

        // Skip the compressed data for now
        entry.archive_offset = binary_reader.position();
        binary_reader.skip(entry.compressed_size);

        if (local_header.flags() & EF_DATA_DESCRIPTOR)
        {
            auto data_descriptor = binary_reader.get<DataDescriptor>();
            entry.uncompressed_size = data_descriptor.uncompressed_size();
            entry.compressed_size = data_descriptor.compressed_size();
        }
    }

    // Central directory starts here
    while (binary_reader.good())
    {
        auto cd_file_header = binary_reader.peek<CentralDirectoryFileHeader>();

        // Check if this is a central directory file header
        if (cd_file_header.signature() != ZIP_CENTRAL_DIR_HEADER_SIG)
        {
            break;
        }

        // Read the central directory entry
        binary_reader.skip(sizeof(CentralDirectoryFileHeader));
        binary_reader.skip(cd_file_header.len_filename());
        binary_reader.skip(cd_file_header.len_extrafield());
        binary_reader.skip(cd_file_header.len_comment());
    }

    // End of file
    le_uint32_t central_dir_end_sig(binary_reader.get<uint32_t>());
    if (central_dir_end_sig() != ZIP_END_OF_CENTRAL_DIR_HEADER_SIG)
    {
        printf("Missing 'central directory end record' signature!\n");
        return;
    }

    _valid = true;
}

void ZipArchive::get_compressed_data(const Entry &entry, Vector<uint8_t> &compressed_data)
{
    // Read the compressed data from the file
    compressed_data.resize(entry.compressed_size);
    FileReader file_reader(_path);
    file_reader.seek(entry.archive_offset, WHENCE_START);
    assert(file_reader.read(compressed_data.raw_storage(), compressed_data.count()) == entry.compressed_size);
}

void ZipArchive::write_entry(const Entry &entry, BinaryWriter &writer, const Vector<uint8_t> &compressed_data)
{
    LocalHeader header;
    header.flags = EF_NONE;
    header.compressed_size = compressed_data.count();
    header.compression = CM_DEFLATED;
    header.uncompressed_size = entry.uncompressed_size;
    header.len_filename = entry.name.length();
    header.len_extrafield = 0;
    header.signature = ZIP_LOCAL_DIR_HEADER_SIG;

    writer.put(header);
    writer.put_fixed_len_string(entry.name);
    writer.put_data(compressed_data.raw_storage(), compressed_data.count());
}

void ZipArchive::write_central_directory(BinaryWriter &writer)
{
    auto start = writer.position();
    for (const auto &entry : _entries)
    {
        CentralDirectoryFileHeader header;
        header.flags = EF_NONE;
        header.compressed_size = entry.compressed_size;
        header.compression = CM_DEFLATED;
        header.uncompressed_size = entry.uncompressed_size;
        header.local_header_offset = entry.archive_offset - sizeof(LocalHeader) - entry.name.length();
        header.len_filename = entry.name.length();
        header.len_extrafield = 0;
        header.len_comment = 0;
        header.signature = ZIP_CENTRAL_DIR_HEADER_SIG;
        writer.put(header);
        writer.put_fixed_len_string(entry.name);
    }
    auto end = writer.position();

    CentralDirectoryEndRecord end_record;
    end_record.signature = ZIP_END_OF_CENTRAL_DIR_HEADER_SIG;
    end_record.central_dir_size = end - start;
    end_record.central_dir_offset = start;
    end_record.disk_entries = _entries.count();
    end_record.total_entries = _entries.count();
    end_record.len_comment = 0;
    writer.put(end_record);
}

Result ZipArchive::extract(unsigned int entry_index, const char *dest_path)
{
    // Read the compressed data from the entry
    const auto &entry = _entries[entry_index];

    if (entry.compression != CM_DEFLATED)
    {
        printf("ZipArchive: Unsupported compression: %u\n", entry.compression);
        return Result::ERR_FUNCTION_NOT_IMPLEMENTED;
    }

    Vector<uint8_t> compressed_data;
    get_compressed_data(entry, compressed_data);

    Vector<uint8_t> uncompressed_data(entry.uncompressed_size);

    Inflate inf;

    // Pre-Allocate the uncompressed output buffer
    auto result = inf.perform(compressed_data, uncompressed_data);
    if (result != Result::SUCCESS)
    {
        return result;
    }

    File dest_file(dest_path);
    return dest_file.write_all(uncompressed_data.raw_storage(), uncompressed_data.count());
}

Result ZipArchive::insert(const char *entry_name, const char *src_path)
{
    File in_file(src_path);
    ResultOr<Slice> result = in_file.read_all();

    if (!result.success())
    {
        return result.result();
    }

    // TODO: create a new entry and write it to the output file
    MemoryWriter memory_writer;
    BinaryWriter binary_Writer(memory_writer);

    // Write local headers
    for (const auto &entry : _entries)
    {
        Vector<uint8_t> entry_compressed_data;
        get_compressed_data(entry, entry_compressed_data);

        write_entry(entry, binary_Writer, entry_compressed_data);
    }

    // Read the uncompressed data from the file
    Slice data = *result;
    Deflate def(5);
    Vector<uint8_t> file_uncompressed_data;
    file_uncompressed_data.push_back_data((uint8_t *)data.start(), data.size());

    // Perform deflate on the data
    Vector<uint8_t> new_compressed_data;
    auto def_result = def.perform(file_uncompressed_data, new_compressed_data);
    if (def_result != Result::SUCCESS)
    {
        return def_result;
    }

    // Write our new entry
    auto &new_entry = _entries.emplace_back();
    new_entry.name = String(entry_name);
    new_entry.compressed_size = new_compressed_data.count();
    new_entry.compression = CM_DEFLATED;
    new_entry.uncompressed_size = data.size();
    new_entry.archive_offset = memory_writer.length() + sizeof(LocalHeader) + new_entry.name.length();

    write_entry(new_entry, binary_Writer, new_compressed_data);

    // Write central directory
    write_central_directory(binary_Writer);

    // Do this properly...
    FileWriter file_writer(_path);
    MemoryReader memory_reader(memory_writer.data());
    file_writer.copy_from(memory_reader);

    return Result::SUCCESS;
}