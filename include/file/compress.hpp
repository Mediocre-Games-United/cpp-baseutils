#pragma once

#include "base_types.hpp"
#include "file.hpp"
#include <zlib.h>
#include <lzma.h>

namespace cbu::compress {
    class error {
    public:
        error(string err) : err(err) {}
        inline string what() { return err; }
    private:
        string err;
    };
}
namespace cbu::compress::gz {
    inline BYTEARRAY compress(BYTEARRAY in,U8 lvl = 5) {
        if (lvl > 9) throw error("GZ Compression failed: Invalid compression level");
        BYTEARRAY res;

        SZ res_size = compressBound_z(in.size());
        res.resize(res_size);

        int code = compress2_z(res.data(),&res_size,in.data(),in.size(),lvl);
        if (code != Z_OK) throw error("GZ Compression failed: Zlib error");

        res.resize(res_size);
        return res;
    }
    inline BYTEARRAY decompress(BYTEARRAY in,SZ buffer_size) {
        BYTEARRAY res{};
        res.resize(buffer_size);

        SZ decomp_size = buffer_size;
        int code = uncompress_z(res.data(),&decomp_size,in.data(),in.size());
        if (code == Z_BUF_ERROR) throw error("GZ Decompression failed: Buffer is too small");
        if (code != Z_OK) throw error("GZ Decompression failed: Zlib error");

        res.resize(decomp_size);
        return res;
    }
}
namespace cbu::compress::xz {
    inline BYTEARRAY compress(const BYTEARRAY& in, U8 lvl = 5) {
        if (lvl > 9)
            throw error("XZ compression failed: Invalid compression level");

        const size_t bound = lzma_stream_buffer_bound(in.size());
        if (bound == 0)
            throw error("XZ compression failed: Input is too large");

        BYTEARRAY result(bound);
        size_t result_pos = 0;

        const lzma_ret code = lzma_easy_buffer_encode(
            static_cast<uint32_t>(lvl),
                                                      LZMA_CHECK_CRC64,
                                                      nullptr,
                                                      in.data(), in.size(),
                                                      result.data(), &result_pos, result.size());

        if (code != LZMA_OK)
            throw error("XZ compression failed: liblzma error");

        result.resize(result_pos);
        return result;
    }

    // buffer_size is the maximum allowed decompressed size.
    inline BYTEARRAY decompress(const BYTEARRAY& in, SZ buffer_size) {
        BYTEARRAY result(buffer_size);
        size_t input_pos = 0;
        size_t output_pos = 0;

        // Set a finite memory limit for decoding.
        uint64_t memlimit = 256ULL * 1024 * 1024;

        const lzma_ret code = lzma_stream_buffer_decode(
            &memlimit,
            0,              // flags
            nullptr,        // allocator
            in.data(), &input_pos, in.size(),
                                                        result.data(), &output_pos, result.size());

        if (code == LZMA_BUF_ERROR)
            throw error("XZ decompression failed: Buffer too small or memory limit exceeded");
        if (code != LZMA_OK)
            throw error("XZ decompression failed: Invalid XZ data or liblzma error");
        if (input_pos != in.size())
            throw error("XZ decompression failed: Trailing data");

        result.resize(output_pos);
        return result;
    }
}

