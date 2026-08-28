// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_progressive_combine_row at pngpread.c:1058:1 in png.h
// png_progressive_combine_row at pngpread.c:1058:1 in png.h
// png_progressive_combine_row at pngpread.c:1058:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
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
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <setjmp.h>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_bytep io_ptr = static_cast<png_bytep>(png_get_io_ptr(png_ptr));
    if (io_ptr == nullptr) {
        png_error(png_ptr, "null io_ptr");
        return;
    }

    struct ReaderState {
        const uint8_t* data;
        size_t size;
        size_t offset;
    };

    ReaderState* state = reinterpret_cast<ReaderState*>(io_ptr);
    if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
        png_error(png_ptr, "read overflow");
        return;
    }

    std::memcpy(outBytes, state->data + state->offset, byteCountToRead);
    state->offset += byteCountToRead;
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_72(const uint8_t *Data, size_t Size) {
    if (Data == nullptr || Size == 0) {
        return 0;
    }

    FILE* fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        std::fwrite(Data, 1, Size, fp);
        std::fclose(fp);
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
    if (png_ptr == nullptr) {
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return 0;
    }

    struct ReaderState {
        const uint8_t* data;
        size_t size;
        size_t offset;
    } state{Data, Size, 0};

    png_set_read_fn(png_ptr, &state, PngReadCallback);

    if (Size > 0) {
        if (Data[0] & 0x01) png_set_gray_to_rgb(png_ptr);
        if (Data[0] & 0x02) png_set_scale_16(png_ptr);
        if (Data[0] & 0x04) png_set_tRNS_to_alpha(png_ptr);
    }

    png_read_info(png_ptr, info_ptr);

    if (Size > 1) {
        if (Data[1] & 0x01) png_set_gray_to_rgb(png_ptr);
        if (Data[1] & 0x02) png_set_scale_16(png_ptr);
        if (Data[1] & 0x04) png_set_tRNS_to_alpha(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    if (Size > 2 && (Data[2] & 0x01)) {
        if (setjmp(png_jmpbuf(png_ptr)) == 0) {
            png_read_update_info(png_ptr, info_ptr);
        }
    }

    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    if (rowbytes == 0 || rowbytes > (1u << 20)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return 0;
    }

    std::vector<png_byte> row(rowbytes);
    std::vector<png_byte> display_row(rowbytes);
    std::vector<png_byte> combine_row(rowbytes);

    png_uint_32 max_rows = height;
    if (max_rows > 64) max_rows = 64;

    for (png_uint_32 y = 0; y < max_rows; ++y) {
        png_bytep row_ptr = (Size > 3 && (Data[3] & 0x01)) ? row.data() : nullptr;
        png_bytep dsp_ptr = (Size > 3 && (Data[3] & 0x02)) ? display_row.data() : nullptr;

        png_read_row(png_ptr, row_ptr, dsp_ptr);

        png_const_bytep new_row_flag = nullptr;
        if (Size > 4 && (Data[4] & 0x01)) {
            new_row_flag = row.data();
        }

        png_progressive_combine_row(png_ptr, combine_row.data(), new_row_flag);

        if (Size > 5 && (Data[5] & 0x01)) {
            png_progressive_combine_row(png_ptr, display_row.data(), new_row_flag);
        }
    }

    if (height == 0 || width == 0) {
        png_progressive_combine_row(png_ptr, combine_row.data(), row.data());
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
}