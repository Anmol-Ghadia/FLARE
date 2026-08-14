#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <string>

static void NoOpWarning(png_structp, png_const_charp) {}

static void NoOpError(png_structp png_ptr, png_const_charp)
{
    if (png_ptr != NULL)
        longjmp(png_jmpbuf(png_ptr), 1);
}

static std::string MakeMessage(const uint8_t* data, size_t size, size_t start, size_t len)
{
    std::string msg;
    if (start >= size)
        return std::string("fuzz");

    size_t end = start + len;
    if (end > size)
        end = size;

    for (size_t i = start; i < end; ++i) {
        char c = static_cast<char>(data[i]);
        if (c == '\0')
            c = 'A';
        msg.push_back(c);
    }

    if (msg.empty())
        msg = "fuzz";

    return msg;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                 NoOpError, NoOpWarning);
    if (png_ptr == NULL)
        return 0;

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 0;
    }

    size_t len1 = Size > 0 ? (Data[0] % 64) : 8;
    size_t len2 = Size > 1 ? (Data[1] % 64) : 8;
    size_t len3 = Size > 2 ? (Data[2] % 64) : 8;
    size_t len4 = Size > 3 ? (Data[3] % 64) : 8;
    size_t off = 4;

    std::string msg1 = MakeMessage(Data, Size, off, len1);
    off += len1;
    std::string msg2 = MakeMessage(Data, Size, off, len2);
    off += len2;
    std::string msg3 = MakeMessage(Data, Size, off, len3);
    off += len3;
    std::string msg4 = MakeMessage(Data, Size, off, len4);

    if (Size > 0) {
        switch (Data[0] % 6) {
            case 0:
                png_warning(png_ptr, msg1.c_str());
                break;
            case 1:
                png_chunk_warning(png_ptr, msg2.c_str());
                break;
            case 2:
                png_benign_error(png_ptr, msg3.c_str());
                break;
            case 3:
                png_chunk_benign_error(png_ptr, msg4.c_str());
                break;
            case 4:
                png_error(png_ptr, msg1.c_str());
                break;
            default:
                png_chunk_error(png_ptr, msg2.c_str());
                break;
        }
    } else {
        png_warning(png_ptr, "fuzz");
    }

    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
}