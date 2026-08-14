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
#include <stdio.h>
#include <setjmp.h>
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    if (info_ptr != nullptr) {
      png_destroy_info_struct(png_ptr, &info_ptr);
    }
    if (png_ptr != nullptr) {
      png_destroy_write_struct(&png_ptr, nullptr);
    }
    return 0;
  }

  png_byte io_buffer[8];
  png_set_write_fn(
      png_ptr, io_buffer,
      [](png_structp pngp, png_bytep data, png_size_t length) {
        png_bytep out = reinterpret_cast<png_bytep>(png_get_io_ptr(pngp));
        if (out != nullptr && data != nullptr) {
          for (png_size_t i = 0; i < length; ++i) out[i % 8] ^= data[i];
        }
      },
      [](png_structp) {});

  png_uint_32 width = 1, height = 1;
  int color_type = PNG_COLOR_TYPE_PALETTE;
  int bit_depth = 8;
  int interlace = PNG_INTERLACE_NONE;
  int compression = PNG_COMPRESSION_TYPE_BASE;
  int filter = PNG_FILTER_TYPE_BASE;

  if (Size > 0) width = (Data[0] % 8) + 1;
  if (Size > 1) height = (Data[1] % 8) + 1;
  if (Size > 2) bit_depth = (Data[2] & 1) ? 8 : 1;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  png_color palette[256];
  png_uint_16 hist[256];
  png_uint_16 palette_entries = 1;
  if (Size > 3) palette_entries = static_cast<png_uint_16>((Data[3] % 16) + 1);

  for (png_uint_16 i = 0; i < palette_entries; ++i) {
    uint8_t r = (Size > 4 + i * 3) ? Data[4 + i * 3] : static_cast<uint8_t>(i);
    uint8_t g = (Size > 5 + i * 3) ? Data[5 + i * 3] : static_cast<uint8_t>(i * 3);
    uint8_t b = (Size > 6 + i * 3) ? Data[6 + i * 3] : static_cast<uint8_t>(i * 7);
    palette[i].red = r;
    palette[i].green = g;
    palette[i].blue = b;
    hist[i] = (Size > 7 + i) ? static_cast<png_uint_16>(Data[7 + i] * 257u)
                             : static_cast<png_uint_16>(i);
  }

  png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);

  png_set_hIST(png_ptr, info_ptr, hist);

  png_uint_16p out_hist = nullptr;
  (void)png_get_hIST(png_ptr, info_ptr, &out_hist);

  png_uint_32 masks[] = {
      PNG_FREE_HIST, PNG_FREE_PLTE, PNG_FREE_TRNS, PNG_FREE_TEXT,
      PNG_FREE_ALL
  };
  int freer = (Size > 8 && (Data[8] & 1)) ? PNG_DESTROY_WILL_FREE_DATA
                                          : PNG_USER_WILL_FREE_DATA;
  png_uint_32 mask = masks[(Size > 9) ? (Data[9] % (sizeof(masks) / sizeof(masks[0]))) : 0];
  png_data_freer(png_ptr, info_ptr, freer, mask);

  int free_num = -1;
  if (Size > 10) {
    uint8_t v = Data[10] % 4;
    free_num = (v == 0) ? -1 : static_cast<int>(v - 1);
  }
  png_free_data(png_ptr, info_ptr, mask, free_num);

  png_write_info(png_ptr, info_ptr);

  png_size_t rowbytes = width;
  std::vector<png_byte> row(rowbytes);
  for (png_uint_32 y = 0; y < height; ++y) {
    for (png_size_t x = 0; x < rowbytes; ++x) {
      size_t idx = 11 + y * rowbytes + x;
      row[x] = (Size > idx) ? static_cast<png_byte>(Data[idx] % palette_entries)
                            : static_cast<png_byte>(x % palette_entries);
    }
    png_write_row(png_ptr, row.data());
  }

  if (Size > 11 && (Data[11] & 1)) {
    png_set_hIST(png_ptr, info_ptr, hist);
    (void)png_get_hIST(png_ptr, info_ptr, &out_hist);
  }

  png_write_end(png_ptr, info_ptr);

  png_destroy_info_struct(png_ptr, &info_ptr);
  png_destroy_write_struct(&png_ptr, nullptr);
  return 0;
}