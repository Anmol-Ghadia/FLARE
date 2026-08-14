#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

static void NoOpWarning(png_structp, png_const_charp) {}

static void NoOpError(png_structp png_ptr, png_const_charp)
{
    if (png_ptr != NULL)
        longjmp(png_jmpbuf(png_ptr), 1);
    abort();
}

static char* MakeMessage(const uint8_t* data, size_t size, size_t start, size_t len)
{
    const char kDefault[] = "fuzz";
    if (start >= size || len == 0) {
        char* out = static_cast<char*>(malloc(sizeof(kDefault)));
        if (out != NULL)
            memcpy(out, kDefault, sizeof(kDefault));
        return out;
    }

    size_t end = start + len;
    if (end > size)
        end = size;

    size_t out_len = end - start;
    if (out_len == 0) {
        char* out = static_cast<char*>(malloc(sizeof(kDefault)));
        if (out != NULL)
            memcpy(out, kDefault, sizeof(kDefault));
        return out;
    }

    char* out = static_cast<char*>(malloc(out_len + 1));
    if (out == NULL)
        return NULL;

    for (size_t i = 0; i < out_len; ++i) {
        char c = static_cast<char>(data[start + i]);
        if (c == '\0')
            c = 'A';
        out[i] = c;
    }
    out[out_len] = '\0';
    return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                 NoOpError, NoOpWarning);
    if (png_ptr == NULL)
        return 0;

    char* msg1 = NULL;
    char* msg2 = NULL;
    char* msg3 = NULL;
    char* msg4 = NULL;

    if (setjmp(png_jmpbuf(png_ptr))) {
        free(msg1);
        free(msg2);
        free(msg3);
        free(msg4);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 0;
    }

    size_t len1 = Size > 0 ? (Data[0] % 64) : 8;
    size_t len2 = Size > 1 ? (Data[1] % 64) : 8;
    size_t len3 = Size > 2 ? (Data[2] % 64) : 8;
    size_t len4 = Size > 3 ? (Data[3] % 64) : 8;
    size_t off = 4;

    msg1 = MakeMessage(Data, Size, off, len1);
    off += len1;
    msg2 = MakeMessage(Data, Size, off, len2);
    off += len2;
    msg3 = MakeMessage(Data, Size, off, len3);
    off += len3;
    msg4 = MakeMessage(Data, Size, off, len4);

    if (msg1 == NULL || msg2 == NULL || msg3 == NULL || msg4 == NULL) {
        free(msg1);
        free(msg2);
        free(msg3);
        free(msg4);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 0;
    }

    if (Size > 0) {
        switch (Data[0] % 6) {
            case 0:
                png_warning(png_ptr, msg1);
                break;
            case 1:
                png_chunk_warning(png_ptr, msg2);
                break;
            case 2:
                png_benign_error(png_ptr, msg3);
                break;
            case 3:
                png_chunk_benign_error(png_ptr, msg4);
                break;
            case 4:
                png_error(png_ptr, msg1);
                break;
            default:
                png_chunk_error(png_ptr, msg2);
                break;
        }
    } else {
        png_warning(png_ptr, "fuzz");
    }

    free(msg1);
    free(msg2);
    free(msg3);
    free(msg4);
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
}