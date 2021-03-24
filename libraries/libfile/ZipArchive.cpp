#include <assert.h>
#include <stdio.h>

#include <libio/File.h>

#include <libcompression/Deflate.h>
#include <libcompression/Inflate.h>
#include <libfile/ZipArchive.h>
#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>
#include <libio/Read.h>
#include <libio/ScopedReader.h>
#include <libio/Skip.h>
#include <libio/Write.h>
#include <libsystem/Logger.h>
#include <libutils/Endian.h>

// Central header
constexpr uint32_t ZIP_END_OF_CENTRAL_DIR_HEADER_SIG = 0x06054b50;
constexpr uint32_t ZIP_CENTRAL_DIR_HEADER_SIG = 0x02014b50;

// Local header
constexpr uint32_t ZIP_LOCAL_DIR_HEADER_SIG = 0x04034b50;

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
        logger_trace("Did read archive, result: %u ", read_archive());
    }
}

Result read_local_headers(IO::SeekableReader auto &reader, Vector<Archive::Entry> &entries)
{
    // Read all local file headers and data descriptors
    while (reader.tell().value() < (reader.length().value() - sizeof(LocalHeader)))
    {
        logger_trace("Read local header");
        auto local_header = TRY(IO::peek<LocalHeader>(reader));

        // Check if this is a local header
        if (local_header.signature() != ZIP_LOCAL_DIR_HEADER_SIG)
        {
            logger_trace("Invalid signature: %u", local_header.signature());
            break;
        }

        auto &entry = entries.emplace_back();
        assert_equal(IO::skip(reader, sizeof(LocalHeader)), SUCCESS);

        // Get the uncompressed & compressed sizes
        entry.uncompressed_size = local_header.uncompressed_size();
        entry.compressed_size = local_header.compressed_size();
        entry.compression = local_header.compression();

        // Read the filename of this entry
        entry.name = TRY(IO::read_string(reader, local_header.len_filename()));
        logger_trace("Found local header: '%s'", entry.name.cstring());

        // Read extra fields
        auto end_position = reader.tell().value() + local_header.len_extrafield();
        while (reader.tell().value() < end_position)
        {
            le_eft extra_field_type(IO::read<ExtraFieldType>(reader).value());
            le_uint16_t extra_field_size(IO::read<uint16_t>(reader).value());

            // TODO: parse the known extra field types
            IO::skip(reader, extra_field_size());
        }

        // Skip the compressed data for now
        entry.archive_offset = reader.tell().value();
        IO::skip(reader, entry.compressed_size);

        if (local_header.flags() & EF_DATA_DESCRIPTOR)
        {
            auto data_descriptor = TRY(IO::read<DataDescriptor>(reader));
            entry.uncompressed_size = data_descriptor.uncompressed_size();
            entry.compressed_size = data_descriptor.compressed_size();
        }
    }

    return Result::SUCCESS;
}

Result read_central_directory(IO::SeekableReader auto &reader)
{
    // Central directory starts here
    while (reader.tell().value() < (reader.length().value() - sizeof(CentralDirectoryFileHeader)))
    {
        logger_trace("Read central directory header");
        auto cd_file_header = TRY(IO::peek<CentralDirectoryFileHeader>(reader));

        // Check if this is a central directory file header
        if (cd_file_header.signature() != ZIP_CENTRAL_DIR_HEADER_SIG)
        {
            break;
        }

        // Read the central directory entry
        TRY(IO::skip(reader, sizeof(CentralDirectoryFileHeader)));

        String name = IO::read_string(reader, cd_file_header.len_filename()).value();
        logger_trace("Found central directory header: '%s'", name.cstring());

        TRY(IO::skip(reader, cd_file_header.len_extrafield()));
        TRY(IO::skip(reader, cd_file_header.len_comment()));
    }

    // End of file
    le_uint32_t central_dir_end_sig = TRY(IO::read<uint32_t>(reader));
    if (central_dir_end_sig() != ZIP_END_OF_CENTRAL_DIR_HEADER_SIG)
    {
        logger_error("Missing 'central directory end record' signature!");
        return Result::ERR_INVALID_DATA;
    }

    return Result::SUCCESS;
}

Result ZipArchive::read_archive()
{
    _valid = false;

    IO::File archive_file(_path, OPEN_READ);

    // Archive does not exist
    if (!archive_file.exist())
    {
        logger_error("Archive does not exist: %s", _path.string().cstring());
        return Result::ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    logger_trace("Opening file: '%s'", _path.string().cstring());

    // A valid zip must atleast contain a "CentralDirectoryEndRecord"
    if (archive_file.length().value() < sizeof(CentralDirectoryEndRecord))
    {
        logger_error("Archive is too small to be a valid .zip: %s %u", _path.string().cstring(), archive_file.length());
        return Result::ERR_INVALID_DATA;
    }

    TRY(read_local_headers(archive_file, _entries));
    TRY(read_central_directory(archive_file));

    _valid = true;
    return Result::SUCCESS;
}

void write_entry(const Archive::Entry &entry, IO::Writer &writer, IO::SeekableReader auto &compressed)
{
    LocalHeader header;
    header.flags = EF_NONE;
    header.compressed_size = compressed.length().value();
    header.compression = CM_DEFLATED;
    header.uncompressed_size = entry.uncompressed_size;
    header.len_filename = entry.name.length();
    header.len_extrafield = 0;
    header.signature = ZIP_LOCAL_DIR_HEADER_SIG;

    // Write data
    IO::write(writer, header);
    IO::write(writer, entry.name);
    IO::copy(compressed, writer);
}

void write_central_directory(IO::SeekableWriter auto &writer, Vector<Archive::Entry> &entries)
{
    auto start = writer.tell();
    for (const auto &entry : entries)
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
        IO::write(writer, header);
        IO::write(writer, entry.name);
    }
    auto end = writer.tell();

    CentralDirectoryEndRecord end_record;
    end_record.signature = ZIP_END_OF_CENTRAL_DIR_HEADER_SIG;
    end_record.central_dir_size = end.value() - start.value();
    end_record.central_dir_offset = start.value();
    end_record.disk_entries = entries.count();
    end_record.total_entries = entries.count();
    end_record.len_comment = 0;
    IO::write(writer, end_record);
}

Result ZipArchive::extract(unsigned int entry_index, const char *dest_path)
{
    // Read the compressed data from the entry
    const auto &entry = _entries[entry_index];

    if (entry.compression != CM_DEFLATED)
    {
        logger_error("ZipArchive: Unsupported compression: %u\n", entry.compression);
        return Result::ERR_NOT_IMPLEMENTED;
    }

    // Get a reader to the uncompressed data
    IO::File file_reader(_path, OPEN_READ);
    file_reader.seek(IO::SeekFrom::start(entry.archive_offset));
    IO::ScopedReader scoped_reader(file_reader, entry.compressed_size);

    // Get a writer to the output
    IO::File file_writer(dest_path, OPEN_WRITE | OPEN_CREATE);

    Compression::Inflate inf;
    return inf.perform(scoped_reader, file_writer).result();
}

Result ZipArchive::insert(const char *entry_name, const char *src_path)
{
    IO::File src_reader{src_path, OPEN_READ};

    if (!src_reader.exist())
    {
        return Result::ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    // TODO: create a new entry and write it to the output file
    IO::MemoryWriter memory_writer;

    // Write local headers
    for (const auto &entry : _entries)
    {
        IO::File file_reader(_path, OPEN_READ);
        file_reader.seek(IO::SeekFrom::start(entry.archive_offset));

        IO::ScopedReader scoped_reader(file_reader, entry.compressed_size);
        logger_trace("Write existing local header: '%s'", entry.name.cstring());
        write_entry(entry, memory_writer, scoped_reader);
    }

    // Get a reader to the original file
    IO::MemoryWriter compressed_writer;

    // Perform deflate on the data
    Compression::Deflate def(5);
    TRY(def.perform(src_reader, compressed_writer));

    // Write our new entry
    logger_trace("Write new local header: '%s'", entry_name);

    auto &new_entry = _entries.emplace_back();
    new_entry.name = String(entry_name);
    new_entry.compressed_size = compressed_writer.length().value();
    new_entry.compression = CM_DEFLATED;
    new_entry.uncompressed_size = src_reader.length().value();
    new_entry.archive_offset = memory_writer.length().value() + sizeof(LocalHeader) + new_entry.name.length();

    auto compressed_data = compressed_writer.slice();
    IO::MemoryReader compressed_reader(compressed_data->start(), compressed_data->size());
    write_entry(new_entry, memory_writer, compressed_reader);

    // Write central directory
    write_central_directory(memory_writer, _entries);

    IO::File file_writer(_path, OPEN_WRITE | OPEN_CREATE);
    return IO::write_all(file_writer, Slice(memory_writer.slice()));
}