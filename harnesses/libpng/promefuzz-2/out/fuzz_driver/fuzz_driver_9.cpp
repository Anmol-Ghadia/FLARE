// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_reset_zstream at png.c:974:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_reset_zstream at png.c:974:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_reset_zstream at png.c:974:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <fstream>
#include <algorithm>

struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
};

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    ReaderState* state = static_cast<ReaderState*>(png_get_io_ptr(png_ptr));
    if (state == NULL) {
        png_error(png_ptr, "null io_ptr");
        return;
    }

    if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
        png_error(png_ptr, "read overflow");
        return;
    }

    memcpy(outBytes, state->data + state->offset, byteCountToRead);
    state->offset += byteCountToRead;
}

static void SilentErrorCallback(png_structp png_ptr, png_const_charp) {
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void SilentWarnCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Data == NULL) {
        return 0;
    }

    {
        std::ofstream ofs("./dummy_file", std::ios::binary);
        if (ofs) {
            ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
        }
    }

    png_set_compression_buffer_size(NULL, 1);
    (void)png_reset_zstream(NULL);
    png_process_data(NULL, NULL, NULL, 0);
    png_read_info(NULL, NULL);
    png_read_image(NULL, NULL);
    png_read_end(NULL, NULL);

    if (Size == 0) {
        return 0;
    }

    const uint8_t control = Data[0];
    const uint8_t* pngData = Data + 1;
    size_t pngSize = Size - 1;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                 SilentErrorCallback, SilentWarnCallback);
    if (png_ptr == NULL) {
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 0;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (end_info == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return 0;
    }

    ReaderState state;
    state.data = pngData;
    state.size = pngSize;
    state.offset = 0;

    png_set_read_fn(png_ptr, &state, PngReadCallback);

    size_t buf_size = 1;
    if (pngSize > 0) {
        buf_size = 1 + (pngData[0] % 64);
        if ((control & 0x02) && pngSize >= sizeof(size_t)) {
            size_t derived = 0;
            memcpy(&derived, pngData, std::min(sizeof(size_t), pngSize));
            derived &= 0x7fffffffU;
            if (derived == 0) {
                derived = 1;
            }
            buf_size = derived;
        }
    }

    png_set_compression_buffer_size(png_ptr, buf_size);
    (void)png_reset_zstream(png_ptr);

    if (control & 0x01) {
        size_t consumed = 0;
        while (consumed < pngSize) {
            size_t chunk = 1 + (pngData[consumed] % 32);
            if (chunk > pngSize - consumed) {
                chunk = pngSize - consumed;
            }
            png_process_data(
                png_ptr,
                info_ptr,
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
                size_t total = static_cast<size_t>(rowbytes) * static_cast<size_t>(height);
                if (total > 0 && total < (1u << 28)) {
                    std::vector<uint8_t> image(total);
                    std::vector<png_bytep> rows(height, NULL);
                    for (png_uint_32 y = 0; y < height; ++y) {
                        rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
                    }
                    png_read_image(png_ptr, rows.data());
                } else {
                    png_read_image(png_ptr, NULL);
                }
            } else {
                png_read_image(png_ptr, NULL);
            }
        } else {
            png_read_image(png_ptr, NULL);
        }
    } else {
        png_read_image(png_ptr, NULL);
    }

    (void)png_reset_zstream(png_ptr);
    png_read_end(png_ptr, end_info);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
}