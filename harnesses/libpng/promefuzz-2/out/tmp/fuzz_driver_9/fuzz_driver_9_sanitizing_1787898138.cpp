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
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <algorithm>

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

    memcpy(outBytes, state->data + state->offset, byteCountToRead);
    state->offset += byteCountToRead;
}

static void PngErrorCallback(png_structp, png_const_charp) {
    longjmp(png_jmpbuf(reinterpret_cast<png_structp>(png_get_error_ptr(reinterpret_cast<png_structp>(nullptr)))), 1);
}

static void PngWarnCallback(png_structp, png_const_charp) {
}

static void SilentErrorCallback(png_structp png_ptr, png_const_charp) {
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void SilentWarnCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Data == nullptr) {
        return 0;
    }

    {
        std::ofstream ofs("./dummy_file", std::ios::binary);
        if (ofs) {
            ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
        }
    }

    if (Size == 0) {
        png_set_compression_buffer_size(nullptr, 1);
        (void)png_reset_zstream(nullptr);
        png_process_data(nullptr, nullptr, nullptr, 0);
        png_read_info(nullptr, nullptr);
        png_read_image(nullptr, nullptr);
        png_read_end(nullptr, nullptr);
        return 0;
    }

    const uint8_t control = Data[0];
    const uint8_t* pngData = Data + 1;
    size_t pngSize = Size - 1;

    png_set_compression_buffer_size(nullptr, 1 + (control % 16));
    (void)png_reset_zstream(nullptr);
    png_process_data(nullptr, nullptr, const_cast<png_bytep>(reinterpret_cast<const png_byte*>(pngData)), pngSize);
    png_read_info(nullptr, nullptr);
    png_read_image(nullptr, nullptr);
    png_read_end(nullptr, nullptr);

    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    png_infop end_info = nullptr;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, SilentErrorCallback, SilentWarnCallback);
    if (png_ptr == nullptr) {
        return 0;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return 0;
    }

    end_info = png_create_info_struct(png_ptr);
    if (end_info == nullptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return 0;
    }

    struct ReaderState {
        const uint8_t* data;
        size_t size;
        size_t offset;
    } state = { pngData, pngSize, 0 };

    png_set_read_fn(png_ptr, &state, PngReadCallback);

    size_t buf_size = 1;
    if (pngSize > 0) {
        buf_size = 1 + (pngData[0] % 64);
        if ((control & 0x02) && pngSize >= sizeof(size_t)) {
            size_t derived = 0;
            memcpy(&derived, pngData, std::min(sizeof(size_t), pngSize));
            derived &= 0x7fffffffU;
            if (derived == 0) derived = 1;
            buf_size = derived;
        }
    }
    png_set_compression_buffer_size(png_ptr, buf_size);
    (void)png_reset_zstream(png_ptr);

    if (control & 0x01) {
        size_t consumed = 0;
        while (consumed < pngSize) {
            size_t chunk = 1 + ((consumed < pngSize ? pngData[consumed] : 0) % 32);
            if (chunk > pngSize - consumed) {
                chunk = pngSize - consumed;
            }
            png_process_data(png_ptr, info_ptr,
                const_cast<png_bytep>(reinterpret_cast<const png_byte*>(pngData + consumed)),
                chunk);
            consumed += chunk;
        }
    }

    png_read_info(png_ptr, info_ptr);

    png_uint_32 width = 0, height = 0;
    int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;

    if (png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_type, &compression_type, &filter_method)) {
        if (width > 0 && height > 0) {
            if (width > 4096) width = 4096;
            if (height > 4096) height = 4096;

            png_read_update_info(png_ptr, info_ptr);

            png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
            if (rowbytes > 0 && rowbytes < (1u << 26)) {
                std::vector<png_bytep> rows(height, nullptr);
                std::vector<uint8_t> image;
                size_t total = static_cast<size_t>(rowbytes) * static_cast<size_t>(height);

                if (total > 0 && total < (1u << 28)) {
                    image.resize(total);
                    for (png_uint_32 y = 0; y < height; ++y) {
                        rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
                    }
                    png_read_image(png_ptr, rows.data());
                } else {
                    png_read_image(png_ptr, nullptr);
                }
            } else {
                png_read_image(png_ptr, nullptr);
            }
        } else {
            png_read_image(png_ptr, nullptr);
        }
    } else {
        png_read_image(png_ptr, nullptr);
    }

    (void)png_reset_zstream(png_ptr);
    png_read_end(png_ptr, end_info);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
}