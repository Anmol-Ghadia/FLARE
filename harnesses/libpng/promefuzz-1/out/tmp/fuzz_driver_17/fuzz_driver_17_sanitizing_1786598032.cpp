#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <png.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <setjmp.h>

static void NoOpWarning(png_structp, png_const_charp) {}

static void NoOpError(png_structp png_ptr, png_const_charp)
{
    if (png_ptr != nullptr)
        longjmp(png_jmpbuf(png_ptr), 1);
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t offset)
{
    if (offset + 4 > size) return 0;
    return (static_cast<png_uint_32>(data[offset]) << 24) |
           (static_cast<png_uint_32>(data[offset + 1]) << 16) |
           (static_cast<png_uint_32>(data[offset + 2]) << 8) |
           (static_cast<png_uint_32>(data[offset + 3]));
}

static std::string MakeMessage(const uint8_t* data, size_t size, size_t start, size_t len)
{
    std::string msg;
    if (start >= size) {
        msg = "fuzz";
    } else {
        size_t n = len;
        if (start + n > size) n = size - start;
        msg.reserve(n + 1);
        for (size_t i = 0; i < n; ++i) {
            unsigned char c = data[start + i];
            if (c == 0) c = 'A';
            msg.push_back(static_cast<char>(c));
        }
        if (msg.empty()) msg = "fuzz";
    }
    return msg;
}

static void SetChunkNameFromData(png_structp png_ptr, const uint8_t* data, size_t size, size_t offset)
{
#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)
    if (png_ptr == nullptr) return;
    png_uint_32 chunk = ReadU32(data, size, offset);
    if (chunk == 0) chunk = png_U32(0x74, 0x45, 0x53, 0x74); /* tESt */
    png_ptr->chunk_name = chunk;
#else
    (void)png_ptr;
    (void)data;
    (void)size;
    (void)offset;
#endif
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, NoOpError, NoOpWarning);
    if (png_ptr == nullptr)
        return 0;

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return 0;
    }

    if (Size > 0) {
        if (Data[0] & 1)
            png_ptr->flags |= PNG_FLAG_BENIGN_ERRORS_WARN;
        else
            png_ptr->flags &= ~PNG_FLAG_BENIGN_ERRORS_WARN;
    }

    SetChunkNameFromData(png_ptr, Data, Size, 1);

    size_t msg1_len = Size > 5 ? (Data[4] % 64) : 16;
    size_t msg2_len = Size > 6 ? (Data[5] % 64) : 16;
    size_t msg3_len = Size > 7 ? (Data[6] % 64) : 16;

    std::string msg1 = MakeMessage(Data, Size, 8, msg1_len);
    std::string msg2 = MakeMessage(Data, Size, 8 + msg1_len, msg2_len);
    std::string msg3 = MakeMessage(Data, Size, 8 + msg1_len + msg2_len, msg3_len);

    png_warning(png_ptr, msg1.c_str());
    png_chunk_warning(png_ptr, msg2.c_str());
    png_benign_error(png_ptr, msg3.c_str());
    png_chunk_benign_error(png_ptr, msg1.c_str());

    if (Size > 0) {
        switch (Data[0] % 4) {
            case 0:
                png_error(png_ptr, msg1.c_str());
                break;
            case 1:
                png_chunk_error(png_ptr, msg2.c_str());
                break;
            case 2:
                png_benign_error(png_ptr, msg3.c_str());
                break;
            case 3:
                png_chunk_benign_error(png_ptr, msg1.c_str());
                break;
        }
    }

    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
}