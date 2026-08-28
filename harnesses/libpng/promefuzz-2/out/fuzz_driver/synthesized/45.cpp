// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_invert_mono at pngtrans.c:250:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_invert_mono at pngtrans.c:250:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
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

static void user_read_data(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    if (io_ptr == nullptr) {
        png_error(png_ptr, "No IO pointer");
        return;
    }

    struct BufferState {
        const uint8_t* data;
        size_t size;
        size_t offset;
    };

    BufferState* state = static_cast<BufferState*>(io_ptr);
    if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
        png_error(png_ptr, "Read beyond end of buffer");
        return;
    }

    for (png_size_t i = 0; i < byteCountToRead; ++i) {
        outBytes[i] = state->data[state->offset + i];
    }
    state->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput_45(const uint8_t *Data, size_t Size) {
    if (Data == nullptr || Size == 0) {
        return 0;
    }

    FILE* fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        std::fwrite(Data, 1, Size, fp);
        std::fclose(fp);
    }

    struct BufferState {
        const uint8_t* data;
        size_t size;
        size_t offset;
    } state = { Data, Size, 0 };

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return 0;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (end_info == nullptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return 0;
    }

    png_set_read_fn(png_ptr, &state, user_read_data);

    if (Size >= 8) {
        png_set_sig_bytes(png_ptr, 0);
    }

    png_read_info(png_ptr, info_ptr);

    if (Size > 0) {
        if (Data[0] & 0x01) png_set_gray_to_rgb(png_ptr);
        if (Data[0] & 0x02) png_set_bgr(png_ptr);
        if (Data[0] & 0x04) png_set_strip_alpha(png_ptr);
        if (Data[0] & 0x08) png_set_invert_mono(png_ptr);
        if (Data[0] & 0x10) png_set_palette_to_rgb(png_ptr);
    }

    if (Size > 1 && (Data[1] & 0x01)) {
        png_set_gray_to_rgb(png_ptr);
    }
    if (Size > 1 && (Data[1] & 0x02)) {
        png_set_bgr(png_ptr);
    }
    if (Size > 1 && (Data[1] & 0x04)) {
        png_set_strip_alpha(png_ptr);
    }
    if (Size > 1 && (Data[1] & 0x08)) {
        png_set_invert_mono(png_ptr);
    }
    if (Size > 1 && (Data[1] & 0x10)) {
        png_set_palette_to_rgb(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);
    png_start_read_image(png_ptr);

    if (Size > 2 && (Data[2] & 0x01)) {
        png_start_read_image(png_ptr);
    }

    png_uint_32 width = 0, height = 0;
    int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, &compression_type, &filter_method);

    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (rowbytes > 0 && height > 0) {
        const png_uint_32 max_rows = 64;
        png_uint_32 rows_to_read = height < max_rows ? height : max_rows;

        std::vector<png_byte> row(rowbytes);
        for (png_uint_32 y = 0; y < rows_to_read; ++y) {
            png_read_row(png_ptr, row.data(), nullptr);
        }
    }

    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
}