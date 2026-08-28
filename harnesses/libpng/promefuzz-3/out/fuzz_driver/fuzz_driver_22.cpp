// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_pixels_per_meter at pngget.c:159:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
// png_get_y_pixels_per_inch at pngget.c:361:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_x_pixels_per_meter at pngget.c:115:1 in png.h
// png_get_y_pixels_per_meter at pngget.c:137:1 in png.h
// png_get_pixels_per_meter at pngget.c:159:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
// png_get_y_pixels_per_inch at pngget.c:361:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_x_pixels_per_meter at pngget.c:115:1 in png.h
// png_get_y_pixels_per_meter at pngget.c:137:1 in png.h
// png_get_pixels_per_meter at pngget.c:159:1 in png.h
// png_get_pixels_per_inch at pngget.c:349:1 in png.h
// png_get_y_pixels_per_inch at pngget.c:361:1 in png.h
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
// png_read_info at pngread.c:92:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_x_pixels_per_meter at pngget.c:115:1 in png.h
// png_get_y_pixels_per_meter at pngget.c:137:1 in png.h
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

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
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

    std::memcpy(outBytes, state->data + state->offset, byteCountToRead);
    state->offset += byteCountToRead;
}

static void WriteDummyFile(const uint8_t* Data, size_t Size) {
    FILE* fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (Size > 0) {
            std::fwrite(Data, 1, Size, fp);
        }
        std::fclose(fp);
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    WriteDummyFile(Data, Size);

    if (Data == nullptr || Size == 0) {
        return 0;
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

    png_set_read_fn(png_ptr, &state, PngReadCallback);

    png_read_info(png_ptr, info_ptr);

    png_uint_32 res_x = 0;
    png_uint_32 res_y = 0;
    int unit_type = 0;

    (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
    (void)png_get_pHYs(png_ptr, info_ptr, &res_x, nullptr, &unit_type);
    (void)png_get_pHYs(png_ptr, info_ptr, nullptr, &res_y, nullptr);
    (void)png_get_pHYs(png_ptr, info_ptr, nullptr, nullptr, nullptr);

    (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
    (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);
    (void)png_get_pixels_per_meter(png_ptr, info_ptr);
    (void)png_get_pixels_per_inch(png_ptr, info_ptr);
    (void)png_get_y_pixels_per_inch(png_ptr, info_ptr);

    png_uint_32 width = 0, height = 0;
    int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
    if (png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_type, &compression_type, &filter_method)) {
        png_read_update_info(png_ptr, info_ptr);

        png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        if (height > 0 && rowbytes > 0 && rowbytes < (1u << 20) && height < (1u << 16)) {
            std::vector<png_byte> image;
            std::vector<png_bytep> rows;

            size_t total = static_cast<size_t>(rowbytes) * static_cast<size_t>(height);
            if (total > 0 && total < (1u << 26)) {
                image.resize(total);
                rows.resize(height);
                for (png_uint_32 i = 0; i < height; ++i) {
                    rows[i] = image.data() + static_cast<size_t>(i) * rowbytes;
                }
                png_read_image(png_ptr, rows.data());

                (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
                (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
                (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);
                (void)png_get_pixels_per_meter(png_ptr, info_ptr);
                (void)png_get_pixels_per_inch(png_ptr, info_ptr);
                (void)png_get_y_pixels_per_inch(png_ptr, info_ptr);
            }
        }
    }

    png_read_end(png_ptr, end_info);

    (void)png_get_pHYs(png_ptr, end_info, &res_x, &res_y, &unit_type);
    (void)png_get_x_pixels_per_meter(png_ptr, end_info);
    (void)png_get_y_pixels_per_meter(png_ptr, end_info);
    (void)png_get_pixels_per_meter(png_ptr, end_info);
    (void)png_get_pixels_per_inch(png_ptr, end_info);
    (void)png_get_y_pixels_per_inch(png_ptr, end_info);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
}