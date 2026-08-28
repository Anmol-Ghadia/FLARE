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

    for (png_size_t i = 0; i < byteCountToRead; ++i) {
        outBytes[i] = state->data[state->offset + i];
    }
    state->offset += byteCountToRead;
}

static void NoopProgressiveInfo(png_structp, png_infop) {}

static void NoopProgressiveEnd(png_structp, png_infop) {}

static void ProgressiveRowCallback(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass) {
    png_infop info_ptr = png_get_progressive_ptr(png_ptr) ? nullptr : nullptr;
    (void)info_ptr;
    png_uint_32 width = png_get_image_width(png_ptr, png_get_info_ptr(png_ptr));
    png_size_t rowbytes = png_get_rowbytes(png_ptr, png_get_info_ptr(png_ptr));
    if (rowbytes == 0) {
        rowbytes = static_cast<png_size_t>(width ? width : 1);
    }
    std::vector<png_byte> old_row(rowbytes);
    png_progressive_combine_row(png_ptr, old_row.data(), new_row);
    if (row_num == 0 && pass == 0) {
        png_progressive_combine_row(png_ptr, old_row.data(), nullptr);
    }
}

static void FuzzReadFromMemory(const uint8_t* Data, size_t Size, bool use_row_mode, bool set_trns, bool set_gray) {
    struct BufferState {
        const uint8_t* data;
        size_t size;
        size_t offset;
    } state{Data, Size, 0};

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return;
    }

    png_set_read_fn(png_ptr, &state, PngReadCallback);

    if (set_trns) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    if (set_gray) {
        png_set_gray_to_rgb(png_ptr);
    }

    png_read_info(png_ptr, info_ptr);

    if (!set_trns) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    if (!set_gray) {
        png_set_gray_to_rgb(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 width = 0, height = 0;
    int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, &compression_type, &filter_method);

    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (rowbytes == 0 || height == 0) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return;
    }

    if (use_row_mode) {
        std::vector<png_byte> row(rowbytes);
        std::vector<png_byte> display_row(rowbytes);
        for (png_uint_32 y = 0; y < height; ++y) {
            png_bytep display = (y & 1) ? display_row.data() : nullptr;
            png_read_row(png_ptr, row.data(), display);
        }
    } else {
        std::vector<std::vector<png_byte>> rows(height, std::vector<png_byte>(rowbytes));
        std::vector<png_bytep> row_ptrs(height);
        for (png_uint_32 y = 0; y < height; ++y) {
            row_ptrs[y] = rows[y].data();
        }
        png_read_image(png_ptr, row_ptrs.data());
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
}

static void FuzzProgressive(const uint8_t* Data, size_t Size) {
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return;
    }

    png_set_progressive_read_fn(png_ptr, nullptr, NoopProgressiveInfo, ProgressiveRowCallback, NoopProgressiveEnd);
    png_set_tRNS_to_alpha(png_ptr);
    png_set_gray_to_rgb(png_ptr);

    size_t offset = 0;
    while (offset < Size) {
        size_t chunk = 1 + (Data[offset] % 32);
        if (chunk > Size - offset) {
            chunk = Size - offset;
        }
        png_process_data(png_ptr, info_ptr, const_cast<png_bytep>(Data + offset), static_cast<png_size_t>(chunk));
        offset += chunk;
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE* fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (Size > 0) {
            std::fwrite(Data, 1, Size, fp);
        }
        std::fclose(fp);
    }

    FuzzReadFromMemory(Data, Size, false, false, false);
    FuzzReadFromMemory(Data, Size, true, true, false);
    FuzzReadFromMemory(Data, Size, true, false, true);
    FuzzReadFromMemory(Data, Size, false, true, true);
    FuzzProgressive(Data, Size);

    return 0;
}