// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_bKGD at pngset.c:25:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
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

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    (void)fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput_59(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  uint32_t width = 1, height = 1;
  int color_type = PNG_COLOR_TYPE_RGB;
  int bit_depth = 8;
  int interlace = PNG_INTERLACE_NONE;
  int compression = PNG_COMPRESSION_TYPE_BASE;
  int filter = PNG_FILTER_TYPE_BASE;

  if (Size >= 1) width = (Data[0] % 8) + 1;
  if (Size >= 2) height = (Data[1] % 8) + 1;
  if (Size >= 3) {
    switch (Data[2] % 5) {
      case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
      case 1: color_type = PNG_COLOR_TYPE_RGB; break;
      case 2: color_type = PNG_COLOR_TYPE_PALETTE; break;
      case 3: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
      default: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
    }
  }
  if (Size >= 4) {
    switch (Data[3] % 4) {
      case 0: bit_depth = 1; break;
      case 1: bit_depth = 2; break;
      case 2: bit_depth = 4; break;
      default: bit_depth = 8; break;
    }
  }

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    bit_depth = (Size >= 4 && (Data[3] & 0x80)) ? 16 : 8;
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    bit_depth = 8;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_color palette[256];
    for (int i = 0; i < 256; ++i) {
      size_t idx = 4 + static_cast<size_t>(i) * 3;
      palette[i].red = (idx < Size) ? Data[idx] : static_cast<png_byte>(i);
      palette[i].green = (idx + 1 < Size) ? Data[idx + 1] : static_cast<png_byte>(255 - i);
      palette[i].blue = (idx + 2 < Size) ? Data[idx + 2] : static_cast<png_byte>(i ^ 0xAA);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, 256);
  }

  png_color_16 background;
  background.index = (Size > 0) ? Data[0] : 0;
  background.red = (Size > 5) ? static_cast<png_uint_16>((Data[4] << 8) | Data[5]) : 0;
  background.green = (Size > 7) ? static_cast<png_uint_16>((Data[6] << 8) | Data[7]) : 0;
  background.blue = (Size > 9) ? static_cast<png_uint_16>((Data[8] << 8) | Data[9]) : 0;
  background.gray = (Size > 11) ? static_cast<png_uint_16>((Data[10] << 8) | Data[11]) : 0;

  png_set_bKGD(png_ptr, info_ptr, &background);

  png_color_16p out_background = nullptr;
  (void)png_get_bKGD(png_ptr, info_ptr, &out_background);

  png_int_32 x_offset = 0;
  png_int_32 y_offset = 0;
  int unit_type = PNG_OFFSET_PIXEL;

  if (Size >= 16) {
    x_offset = static_cast<png_int_32>(
        (static_cast<uint32_t>(Data[12]) << 24) |
        (static_cast<uint32_t>(Data[13]) << 16) |
        (static_cast<uint32_t>(Data[14]) << 8) |
        static_cast<uint32_t>(Data[15]));
  }
  if (Size >= 20) {
    y_offset = static_cast<png_int_32>(
        (static_cast<uint32_t>(Data[16]) << 24) |
        (static_cast<uint32_t>(Data[17]) << 16) |
        (static_cast<uint32_t>(Data[18]) << 8) |
        static_cast<uint32_t>(Data[19]));
  }
  if (Size >= 21) {
    unit_type = (Data[20] & 1) ? PNG_OFFSET_MICROMETER : PNG_OFFSET_PIXEL;
  }

  png_set_oFFs(png_ptr, info_ptr, x_offset, y_offset, unit_type);

  png_write_info(png_ptr, info_ptr);

  int channels = 3;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 3; break;
  }

  size_t bytes_per_sample = (bit_depth == 16) ? 2u : 1u;
  size_t rowbytes = static_cast<size_t>(width) * static_cast<size_t>(channels) * bytes_per_sample;
  if (bit_depth < 8 && (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)) {
    rowbytes = (static_cast<size_t>(width) * static_cast<size_t>(bit_depth) + 7u) / 8u;
  }

  std::vector<uint8_t> row(rowbytes ? rowbytes : 1u);
  size_t data_off = 21;

  for (uint32_t y = 0; y < height; ++y) {
    for (size_t i = 0; i < rowbytes; ++i) {
      row[i] = (data_off + i < Size) ? Data[data_off + i] : static_cast<uint8_t>((y + i) & 0xFF);
    }
    png_write_row(png_ptr, row.data());
    if (data_off < Size) {
      data_off += rowbytes;
      if (data_off > Size) data_off = Size;
    }
  }

  if (Size & 1) {
    (void)png_get_bKGD(png_ptr, info_ptr, &out_background);
  }

  png_write_end(png_ptr, info_ptr);

  png_destroy_info_struct(png_ptr, &info_ptr);
  png_destroy_write_struct(&png_ptr, nullptr);
  fclose(fp);
  return 0;
}