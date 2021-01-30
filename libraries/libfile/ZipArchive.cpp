#include <libfile/ZipArchive.h>
#include <libsystem/Logger.h>
#include <libsystem/compression/Deflate.h>
#include <libsystem/compression/Inflate.h>
#include <libutils/BinaryReader.h>
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

ZipArchive::ZipArchive(File file, bool read) : Archive(file)
{
    if (read)
    {
        read_archive();
    }
}

void ZipArchive::read_archive()
{
    _valid = false;
    Result result = _file.read_all((void **)&_data, &_size);

    // Failed to read the file
    if (result != Result::SUCCESS)
    {
        printf("Failed to read archive: %s", _file.path().string().cstring());
        return;
    }

    // A valid zip must atleast contain a "CentralDirectoryEndRecord"
    if (_size < sizeof(CentralDirectoryEndRecord))
    {
        printf("Archive is too small to be a valid .zip: %s %u", _file.path().string().cstring(), _size);
        return;
    }

    Slice content(_data, _size);
    BinaryReader reader(content);

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

    // Central directory starts here
    while (reader.good())
    {
        auto cd_file_header = reader.peek<CentralDirectoryFileHeader>();

        // Check if this is a central directory file header
        if (cd_file_header.signature() != ZIP_CENTRAL_DIR_HEADER_SIG)
        {
            break;
        }

        // Read the central directory entry
        reader.skip(sizeof(CentralDirectoryFileHeader));
        reader.skip(cd_file_header.len_filename());
        reader.skip(cd_file_header.len_extrafield());
        reader.skip(cd_file_header.len_comment());
    }

    // End of file
    le_uint32_t central_dir_end_sig(reader.get<uint32_t>());
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
    Slice slice_compressed = Slice(_data + entry.archive_offset, entry.compressed_size);
    compressed_data = Vector<uint8_t>(ADOPT, (uint8_t *)slice_compressed.start(), slice_compressed.size());
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
    auto start = writer.pos();
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
    auto end = writer.pos();

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
    const auto &entry = get_entry(entry_index);

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
    Vector<uint8_t> archive_data;
    BinaryWriter archive_writer(archive_data);

    // Write local headers
    for (const auto &entry : _entries)
    {
        Vector<uint8_t> entry_compressed_data;
        get_compressed_data(entry, entry_compressed_data);

        write_entry(entry, archive_writer, entry_compressed_data);
    }

    // Read the uncompressed data from the file
    Slice data = *result;
    Deflate def(5);
    Vector<uint8_t> file_uncompressed_data(ADOPT, (uint8_t *)data.start(), data.size());

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
    new_entry.archive_offset = archive_data.count() + sizeof(LocalHeader) + new_entry.name.length();

    write_entry(new_entry, archive_writer, new_compressed_data);

    // Write central directory
    write_central_directory(archive_writer);

    // Do this properly...
    _file.write_all(archive_data.raw_storage(), archive_data.count());
    if (_data != nullptr)
        delete[] _data;
    return _file.read_all((void **)&_data, &_size);
}