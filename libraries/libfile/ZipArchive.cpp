#include <libfile/ZipArchive.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/compression/Deflate.h>
#include <libsystem/compression/Inflate.h>
#include <libsystem/io/BinaryReader.h>
#include <libsystem/io/FileReader.h>
#include <libsystem/io/FileWriter.h>
#include <libsystem/io/MemoryReader.h>
#include <libsystem/io/MemoryWriter.h>
#include <libsystem/io/ScopedReader.h>
#include <libutils/Endian.h>

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

void ZipArchive::read_local_headers(BinaryReader &reader)
{
    // Read all local file headers and data descriptors
    while (reader.position() < (reader.length() - sizeof(LocalHeader)))
    {
        logger_trace("Read local header: '%s'", _path.string().cstring());
        auto local_header = reader.peek<LocalHeader>();

        // Check if this is a local header
        if (local_header.signature() != ZIP_LOCAL_DIR_HEADER_SIG)
        {
            logger_trace("Invalid signature: %u", local_header.signature());
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
        logger_trace("Found local header: '%s'", entry.name.cstring());

        // Read extra fields
        auto end_position = reader.position() + local_header.len_extrafield();
        while (reader.position() < end_position)
        {
            le_eft extra_field_type(reader.get<ExtraFieldType>());
            le_uint16_t extra_field_size(reader.get<uint16_t>());

            // TODO: parse the known extra field types
            reader.skip(extra_field_size());
        }

        // Skip the compressed data for now
        entry.archive_offset = reader.position();
        reader.skip(entry.compressed_size);

        if (local_header.flags() & EF_DATA_DESCRIPTOR)
        {
            auto data_descriptor = reader.get<DataDescriptor>();
            entry.uncompressed_size = data_descriptor.uncompressed_size();
            entry.compressed_size = data_descriptor.compressed_size();
        }
    }
}

Result ZipArchive::read_central_directory(BinaryReader &reader)
{
    // Central directory starts here
    while (reader.position() < (reader.length() - sizeof(CentralDirectoryFileHeader)))
    {
        logger_trace("Read central directory header: '%s'", _path.string().cstring());
        auto cd_file_header = reader.peek<CentralDirectoryFileHeader>();

        // Check if this is a central directory file header
        if (cd_file_header.signature() != ZIP_CENTRAL_DIR_HEADER_SIG)
        {
            break;
        }

        // Read the central directory entry
        reader.skip(sizeof(CentralDirectoryFileHeader));

        String name = reader.get_fixed_len_string(cd_file_header.len_filename());
        logger_trace("Found central directory header: '%s'", name.cstring());

        reader.skip(cd_file_header.len_extrafield());
        reader.skip(cd_file_header.len_comment());
    }

    // End of file
    le_uint32_t central_dir_end_sig(reader.get<uint32_t>());
    if (central_dir_end_sig() != ZIP_END_OF_CENTRAL_DIR_HEADER_SIG)
    {
        printf("Missing 'central directory end record' signature!\n");
        return Result::ERR_INVALID_DATA;
    }

    return Result::SUCCESS;
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

    logger_trace("Opening file: '%s'", _path.string().cstring());

    FileReader file_reader(_path);

    // A valid zip must atleast contain a "CentralDirectoryEndRecord"
    if (file_reader.length() < sizeof(CentralDirectoryEndRecord))
    {
        printf("Archive is too small to be a valid .zip: %s %u", _path.string().cstring(), file_reader.length());
        return;
    }

    BinaryReader binary_reader(file_reader);

    read_local_headers(binary_reader);
    Result result = read_central_directory(binary_reader);

    if (result != Result::SUCCESS)
    {
        return;
    }

    _valid = true;
}

void ZipArchive::write_entry(const Entry &entry, BinaryWriter &writer, Reader &compressed)
{
    LocalHeader header;
    header.flags = EF_NONE;
    header.compressed_size = compressed.length();
    header.compression = CM_DEFLATED;
    header.uncompressed_size = entry.uncompressed_size;
    header.len_filename = entry.name.length();
    header.len_extrafield = 0;
    header.signature = ZIP_LOCAL_DIR_HEADER_SIG;

    // Write data
    writer.put(header);
    writer.put_fixed_len_string(entry.name);
    writer.copy_from(compressed);
}

void ZipArchive::write_central_directory(BinaryWriter &writer)
{
    auto start = writer.position();
    for (const auto &entry : _entries)
    {
        logger_trace("Write central directory header: '%s'", entry.name.cstring());
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
    writer.flush();
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

    Inflate inf;

    // Get a reader to the uncompressed data
    FileReader file_reader(_path);
    file_reader.seek(entry.archive_offset, WHENCE_START);
    ScopedReader scoped_reader(file_reader, entry.uncompressed_size);

    // Get a writer to the output
    FileWriter file_writer(dest_path);

    return inf.perform(scoped_reader, file_writer);
}

Result ZipArchive::insert(const char *entry_name, const char *src_path)
{
    File src_file(src_path);

    if (!src_file.exist())
    {
        return Result::ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    // TODO: create a new entry and write it to the output file
    MemoryWriter memory_writer;
    BinaryWriter binary_writer(memory_writer);

    // Write local headers
    for (const auto &entry : _entries)
    {
        FileReader file_reader(_path);
        file_reader.seek(entry.archive_offset, WHENCE_START);
        ScopedReader scoped_reader(file_reader, entry.compressed_size);
        logger_trace("Write existing local header: '%s'", entry.name.cstring());
        write_entry(entry, binary_writer, scoped_reader);
    }

    // Get a reader to the original file
    FileReader src_reader(src_path);
    MemoryWriter compressed_writer;

    // Perform deflate on the data
    Deflate def(5);
    auto def_result = def.perform(src_reader, compressed_writer);
    if (def_result != Result::SUCCESS)
    {
        return def_result;
    }

    // Write our new entry
    logger_trace("Write new local header: '%s'", entry_name);

    auto &new_entry = _entries.emplace_back();
    new_entry.name = String(entry_name);
    new_entry.compressed_size = compressed_writer.length();
    new_entry.compression = CM_DEFLATED;
    new_entry.uncompressed_size = src_reader.length();
    new_entry.archive_offset = memory_writer.length() + sizeof(LocalHeader) + new_entry.name.length();

    MemoryReader compressed_reader(compressed_writer.data());
    write_entry(new_entry, binary_writer, compressed_reader);

    // Write central directory
    write_central_directory(binary_writer);

    // Do this properly...
    FileWriter file_writer(_path);
    MemoryReader memory_reader(memory_writer.data());
    file_writer.copy_from(memory_reader);

    return Result::SUCCESS;
}