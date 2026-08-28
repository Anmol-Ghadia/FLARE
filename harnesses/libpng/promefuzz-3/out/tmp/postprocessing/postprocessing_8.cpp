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
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <setjmp.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
    (void)png_ptr;
    (void)data;
    (void)length;
}

static void PngFlushCallback(png_structp png_ptr) {
    (void)png_ptr;
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp error_msg) {
    (void)error_msg;
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp png_ptr, png_const_charp warning_msg) {
    (void)png_ptr;
    (void)warning_msg;
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) {
        v <<= 8;
        if (off < size) v |= data[off++];
    }
    return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (Data != nullptr && Size > 0) {
            std::fwrite(Data, 1, Size, fp);
        }
        std::fclose(fp);
    }

    size_t off = 0;

    int color_types[] = {
        PNG_COLOR_TYPE_GRAY,
        PNG_COLOR_TYPE_RGB,
        PNG_COLOR_TYPE_PALETTE,
        PNG_COLOR_TYPE_GRAY_ALPHA,
        PNG_COLOR_TYPE_RGB_ALPHA
    };
    int bit_depths[] = {1, 2, 4, 8, 16};
    int interlaces[] = {PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7};
    int compressions[] = {PNG_COMPRESSION_TYPE_BASE};
    int filters[] = {PNG_FILTER_TYPE_BASE};

    uint32_t raw_w = ReadU32(Data, Size, off);
    uint32_t raw_h = ReadU32(Data, Size, off);
    uint32_t width = (raw_w % 32) + 1;
    uint32_t height = (raw_h % 32) + 1;

    int color_type = color_types[(off < Size) ? (Data[off++] % (sizeof(color_types) / sizeof(color_types[0]))) : 0];
    int bit_depth = bit_depths[(off < Size) ? (Data[off++] % (sizeof(bit_depths) / sizeof(bit_depths[0]))) : 0];
    int interlace = interlaces[(off < Size) ? (Data[off++] % (sizeof(interlaces) / sizeof(interlaces[0]))) : 0];
    int compression = compressions[(off < Size) ? (Data[off++] % (sizeof(compressions) / sizeof(compressions[0]))) : 0];
    int filter = filters[(off < Size) ? (Data[off++] % (sizeof(filters) / sizeof(filters[0]))) : 0];

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                  PngErrorCallback, PngWarningCallback);
    if (png_ptr == nullptr) {
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }

    png_set_write_fn(png_ptr, nullptr, PngWriteCallback, PngFlushCallback);

    if ((off < Size) && (Data[off++] & 1)) {
        png_set_sig_bytes(png_ptr, (int)((off < Size) ? (Data[off++] % 9) : 0));
    }

    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
                 interlace, compression, filter);

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_color palette[256];
        for (int i = 0; i < 256; ++i) {
            palette[i].red = (png_byte)((off < Size) ? Data[off++] : i);
            palette[i].green = (png_byte)((off < Size) ? Data[off++] : i);
            palette[i].blue = (png_byte)((off < Size) ? Data[off++] : i);
        }
        int palette_size = (int)(((off < Size) ? Data[off++] : 0) % 256);
        if (palette_size == 0) palette_size = 1;
        png_set_PLTE(png_ptr, info_ptr, palette, palette_size);
    }

    int channels = 0;
    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY: channels = 1; break;
        case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
        case PNG_COLOR_TYPE_RGB: channels = 3; break;
        case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
        case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
        default: channels = 1; break;
    }

    size_t bits_per_pixel = (size_t)channels * (size_t)bit_depth;
    size_t rowbytes = (width * bits_per_pixel + 7) / 8;
    if (rowbytes == 0) rowbytes = 1;

    std::vector<png_byte> image_storage;
    std::vector<png_bytep> rows;
    image_storage.resize(rowbytes * height);
    rows.resize(height);

    for (uint32_t y = 0; y < height; ++y) {
        rows[y] = image_storage.data() + y * rowbytes;
        for (size_t x = 0; x < rowbytes; ++x) {
            rows[y][x] = (png_byte)((off < Size) ? Data[off++] : (uint8_t)(x + y));
        }
    }

    int mode = (off < Size) ? (Data[off++] % 5) : 0;

    switch (mode) {
        case 0:
            png_write_info_before_PLTE(png_ptr, info_ptr);
            png_write_sig(png_ptr);
            png_write_image(png_ptr, rows.data());
            png_write_flush(png_ptr);
            break;

        case 1:
            png_write_sig(png_ptr);
            png_write_info_before_PLTE(png_ptr, info_ptr);
            for (uint32_t y = 0; y < height; ++y) {
                png_write_row(png_ptr, rows[y]);
                if ((y % 4) == 0) {
                    png_write_flush(png_ptr);
                }
            }
            break;

        case 2:
            png_write_info_before_PLTE(png_ptr, info_ptr);
            png_write_info_before_PLTE(png_ptr, info_ptr);
            png_write_sig(png_ptr);
            {
                uint32_t pos = 0;
                while (pos < height) {
                    uint32_t chunk = ((off < Size) ? (Data[off++] % 4) : 0) + 1;
                    if (chunk > height - pos) chunk = height - pos;
                    png_write_rows(png_ptr, rows.data() + pos, chunk);
                    pos += chunk;
                    png_write_flush(png_ptr);
                }
            }
            break;

        case 3:
            png_write_sig(png_ptr);
            png_write_sig(png_ptr);
            png_write_info_before_PLTE(png_ptr, info_ptr);
            png_write_image(png_ptr, rows.data());
            png_write_flush(png_ptr);
            png_write_flush(png_ptr);
            break;

        case 4:
        default:
            png_write_info_before_PLTE(png_ptr, info_ptr);
            if ((off < Size) && (Data[off++] & 1)) {
                int passes = png_set_interlace_handling(png_ptr);
                (void)passes;
            }
            if ((off < Size) && (Data[off++] & 1)) {
                png_write_image(png_ptr, rows.data());
            } else {
                for (uint32_t y = 0; y < height; ++y) {
                    if ((off < Size) && (Data[off++] & 1)) {
                        png_write_rows(png_ptr, rows.data() + y, 1);
                    } else {
                        png_write_row(png_ptr, rows[y]);
                    }
                }
            }
            png_write_flush(png_ptr);
            break;
    }

    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
}