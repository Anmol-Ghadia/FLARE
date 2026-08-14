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
#include <cstring>
#include <setjmp.h>

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

    if (byteCountToRead > 0) {
        memcpy(outBytes, state->data + state->offset, byteCountToRead);
        state->offset += byteCountToRead;
    }
}

static void ExerciseGetters(png_structp png_ptr, png_infop info_ptr) {
    png_uint_32 res_x = 0, res_y = 0;
    int unit_type = 0;

    (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
    (void)png_get_pHYs(png_ptr, info_ptr, &res_x, nullptr, nullptr);
    (void)png_get_pHYs(png_ptr, info_ptr, nullptr, &res_y, nullptr);
    (void)png_get_pHYs(png_ptr, info_ptr, nullptr, nullptr, &unit_type);
    (void)png_get_pHYs(png_ptr, info_ptr, nullptr, nullptr, nullptr);

    (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
    (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);

    (void)png_get_x_offset_inches(png_ptr, info_ptr);
    (void)png_get_y_offset_inches(png_ptr, info_ptr);

#ifdef PNG_FIXED_POINT_SUPPORTED
    (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
    (void)png_get_y_offset_inches_fixed(png_ptr, info_ptr);
#endif

#ifdef PNG_pHYs_SUPPORTED
    res_x = res_y = 0;
    unit_type = 0;
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, nullptr, nullptr);
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, &res_y, nullptr);
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, nullptr, &unit_type);
    (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, nullptr, nullptr);
#endif
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Data == nullptr || Size == 0) {
        return 0;
    }

    {
        FILE* fp = fopen("./dummy_file", "wb");
        if (fp != nullptr) {
            fwrite(Data, 1, Size, fp);
            fclose(fp);
        }
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr != nullptr) {
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr != nullptr) {
            if (setjmp(png_jmpbuf(png_ptr)) == 0) {
                struct BufferState {
                    const uint8_t* data;
                    size_t size;
                    size_t offset;
                } state = { Data, Size, 0 };

                png_set_read_fn(png_ptr, &state, PngReadCallback);
                png_read_info(png_ptr, info_ptr);
                ExerciseGetters(png_ptr, info_ptr);
            }
            png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        } else {
            png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        }
    }

    FILE* fp = fopen("./dummy_file", "rb");
    if (fp != nullptr) {
        png_structp file_png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (file_png_ptr != nullptr) {
            png_infop file_info_ptr = png_create_info_struct(file_png_ptr);
            if (file_info_ptr != nullptr) {
                if (setjmp(png_jmpbuf(file_png_ptr)) == 0) {
                    png_init_io(file_png_ptr, fp);
                    png_read_info(file_png_ptr, file_info_ptr);
                    ExerciseGetters(file_png_ptr, file_info_ptr);
                }
                png_destroy_read_struct(&file_png_ptr, &file_info_ptr, nullptr);
            } else {
                png_destroy_read_struct(&file_png_ptr, nullptr, nullptr);
            }
        }
        fclose(fp);
    }

    return 0;
}