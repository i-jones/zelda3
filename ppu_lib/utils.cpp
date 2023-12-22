#include "utils.hpp"

#include <algorithm>
#include <fstream>
#include <string>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/zstd.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>

bool Utils::areImagesEqual(std::span<const std::uint8_t> a, std::span<const std::uint8_t> b)
{
    return std::equal(a.begin(), a.end(), b.begin(), b.end());
}

namespace io = boost::iostreams;

bool Utils::writeBinary(std::string_view fileName, std::span<const std::uint8_t> data, CompressionType compression)
{
    switch (compression)
    {
    case CompressionType::None:
    {
        std::ofstream recordingFile(fileName, std::ios::binary);
        recordingFile.write(reinterpret_cast<const char *>(data.data()), data.size_bytes());
        break;
    }
    case CompressionType::Gzip:
    {

        std::string gzName = std::string(fileName) + ".gz";

        io::gzip_compressor compressor;
        io::filtering_ostream out;
        out.push(compressor);
        out.push(io::file_sink(gzName, std::ios_base::binary | std::ios_base::out));

        out.write(reinterpret_cast<const char *>(data.data()), data.size_bytes());
        break;
    }
    case CompressionType::Zstd:
    {
        std::string gzName = std::string(fileName) + ".zstd";

        io::zstd_compressor compressor;
        io::filtering_ostream out;
        out.push(compressor);
        out.push(io::file_sink(gzName, std::ios_base::binary | std::ios_base::out));

        out.write(reinterpret_cast<const char *>(data.data()), data.size_bytes());
    }
    }

    return true;
}

std::vector<unsigned char> Utils::loadFile(const std::string &fileName)
{
    if (fileName.ends_with(".gz"))
    {
        io::filtering_istream in;
        io::gzip_decompressor decompressor;
        in.push(decompressor);
        in.push(io::file_source(fileName, std::ios::binary | std::ios::in));

        // copies all data into buffer
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(in), {});

        return buffer;
    }
    else if (fileName.ends_with(".zstd"))
    {
        io::filtering_istream in;
        io::zstd_decompressor decompressor;
        in.push(decompressor);
        in.push(io::file_source(fileName, std::ios::binary | std::ios::in));

        // copies all data into buffer
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(in), {});

        return buffer;
    }
    else
    {
        // Load the recording
        std::ifstream input(fileName, std::ios::binary);

        // copies all data into buffer
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
        input.close();

        return buffer;
    }
}