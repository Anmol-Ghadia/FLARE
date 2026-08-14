#include <png.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
};

static void UserReadData(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    BufferState* state = static_cast<BufferState*>(png_get_io_ptr(png_ptr));
    if (state == nullptr || outBytes == nullptr) {
        png_error(png_ptr, "Invalid IO state");
        return;
    }

    if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
        png_error(png_ptr, "Read beyond end of buffer");
        return;
    }

    memcpy(outBytes, state->data + state->offset, byteCountToRead);
    state->offset += byteCountToRead;
}

static void ExerciseGetters(png_structp png_ptr, png_infop info_ptr) {
    (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
    (void)png_get_pixels_per_meter(png_ptr, info_ptr);
    (void)png_get_x_pixels_per_inch(png_ptr, info_ptr);
    (void)png_get_pixels_per_inch(png_ptr, info_ptr);

    (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs);
    (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_IDAT);
#ifdef PNG_INFO_tIME
    (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_tIME);
#endif
#ifdef PNG_INFO_gAMA
    (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA);
#endif
#ifdef PNG_INFO_tRNS
    (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS);
#endif

    png_uint_32 rx = 0, ry = 0;
    int unit = 0;
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &rx, &ry, &unit);
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &rx, nullptr, &unit);
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, &ry, nullptr);
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, nullptr, nullptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE* fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

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

    ExerciseGetters(png_ptr, info_ptr);

    if (setjmp(png_jmpbuf(png_ptr)) != 0) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return 0;
    }

    BufferState state = { Data, Size, 0 };
    png_set_read_fn(png_ptr, &state, UserReadData);

    png_read_info(png_ptr, info_ptr);
    ExerciseGetters(png_ptr, info_ptr);

    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);
    int interlace_type = png_get_interlace_type(png_ptr, info_ptr);

    if (bit_depth == 16) png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
#ifdef PNG_INFO_tRNS
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
#endif
    if (interlace_type != PNG_INTERLACE_NONE) png_set_interlace_handling(png_ptr);

    png_read_update_info(png_ptr, info_ptr);
    ExerciseGetters(png_ptr, info_ptr);

    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    if (height > 0 && rowbytes > 0 && rowbytes < (1U << 20) && height < (1U << 16)) {
        png_bytep row = static_cast<png_bytep>(malloc(rowbytes));
        if (row != nullptr) {
            for (png_uint_32 y = 0; y < height; ++y) {
                png_read_row(png_ptr, row, nullptr);
                if (y == 0 || y + 1 == height || (y % 7) == 0) {
                    ExerciseGetters(png_ptr, info_ptr);
                }
            }
            free(row);
        }
    }

    png_read_end(png_ptr, end_info);
    ExerciseGetters(png_ptr, info_ptr);
    ExerciseGetters(png_ptr, end_info);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
}