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
        png_error(png_ptr, "no io_ptr");
        return;
    }

    struct BufferState {
        const uint8_t* data;
        size_t size;
        size_t offset;
    };

    BufferState* state = static_cast<BufferState*>(io_ptr);
    if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
        png_error(png_ptr, "read overflow");
        return;
    }

    if (byteCountToRead > 0) {
        memcpy(outBytes, state->data + state->offset, byteCountToRead);
        state->offset += byteCountToRead;
    }
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Data == nullptr || Size == 0) {
        return 0;
    }

    FILE* fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    struct BufferState {
        const uint8_t* data;
        size_t size;
        size_t offset;
    } state = { Data, Size, 0 };

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                 PngErrorCallback, PngWarningCallback);
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

    if (Size > 0) {
        if (Data[0] & 0x01) png_set_gray_to_rgb(png_ptr);
        if (Data[0] & 0x02) png_set_expand(png_ptr);
        if (Data[0] & 0x04) png_set_palette_to_rgb(png_ptr);
    }

    int error_action = 0;
    if (Size > 1) {
        error_action = static_cast<int>(Data[1] % 3);
    }

    if (Size > 9) {
        png_fixed_point red_fixed =
            static_cast<png_fixed_point>((static_cast<uint32_t>(Data[2]) << 24) |
                                         (static_cast<uint32_t>(Data[3]) << 16) |
                                         (static_cast<uint32_t>(Data[4]) << 8) |
                                         static_cast<uint32_t>(Data[5]));
        png_fixed_point green_fixed =
            static_cast<png_fixed_point>((static_cast<uint32_t>(Data[6]) << 24) |
                                         (static_cast<uint32_t>(Data[7]) << 16) |
                                         (static_cast<uint32_t>(Data[8]) << 8) |
                                         static_cast<uint32_t>(Data[9]));
        png_set_rgb_to_gray_fixed(png_ptr, error_action, red_fixed, green_fixed);
    } else {
        png_set_rgb_to_gray_fixed(png_ptr, error_action, 21268, 71514);
    }

    if (Size > 17) {
        double red = static_cast<double>(Data[10]) / 255.0;
        double green = static_cast<double>(Data[11]) / 255.0;
        if (Data[12] & 1) red *= -1.0;
        if (Data[13] & 1) green *= -1.0;
        if (Data[14] & 1) red *= 10.0;
        if (Data[15] & 1) green *= 10.0;
        png_set_rgb_to_gray(png_ptr, error_action, red, green);
    } else {
        png_set_rgb_to_gray(png_ptr, error_action, 0.299, 0.587);
    }

    png_read_update_info(png_ptr, info_ptr);

    if (Size > 16 && (Data[16] & 1)) {
        png_read_update_info(png_ptr, info_ptr);
    }

    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    if (rowbytes > 0 && width > 0 && height > 0) {
        const png_uint_32 max_rows = 1024;
        png_uint_32 rows_to_read = height < max_rows ? height : max_rows;
        std::vector<png_byte> row(rowbytes);

        for (png_uint_32 y = 0; y < rows_to_read; ++y) {
            png_read_row(png_ptr, row.data(), nullptr);
        }

        png_byte status = png_get_rgb_to_gray_status(png_ptr);
        (void)status;
    }

    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
}