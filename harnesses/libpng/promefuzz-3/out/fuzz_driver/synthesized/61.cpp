// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_bKGD at pngset.c:25:1 in png.h
// png_set_sRGB at pngset.c:653:1 in png.h
// png_set_pHYs at pngset.c:556:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
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
#include <string.h>
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t *off) {
  if (*off + 4 > size) {
    *off = size;
    return 0;
  }
  uint32_t v = ((uint32_t)data[*off] << 24) |
               ((uint32_t)data[*off + 1] << 16) |
               ((uint32_t)data[*off + 2] << 8) |
               (uint32_t)data[*off + 3];
  *off += 4;
  return v;
}

static uint16_t ReadU16(const uint8_t *data, size_t size, size_t *off) {
  if (*off + 2 > size) {
    *off = size;
    return 0;
  }
  uint16_t v = (uint16_t)(((uint16_t)data[*off] << 8) |
                          (uint16_t)data[*off + 1]);
  *off += 2;
  return v;
}

extern "C" int LLVMFuzzerTestOneInput_61(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }
  if (Size > 0) {
    fwrite(Data, 1, Size, fp);
  }
  fclose(fp);

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
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
    remove("./dummy_file");
    return 0;
  }

  fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_init_io(png_ptr, fp);

  uint32_t width = (ReadU32(Data, Size, &off) % 8) + 1;
  uint32_t height = (ReadU32(Data, Size, &off) % 8) + 1;

  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int bit_depth_choices[] = {1, 2, 4, 8, 16};

  int color_type = color_type_choices[(off < Size ? Data[off++] : 0) % 5];
  int bit_depth = bit_depth_choices[(off < Size ? Data[off++] : 0) % 5];

  if ((color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) &&
      bit_depth < 8) {
    bit_depth = 8;
  }

  int interlace = (off < Size && (Data[off++] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression = PNG_COMPRESSION_TYPE_BASE;
  int filter = PNG_FILTER_TYPE_BASE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int entries = (int)((off < Size ? Data[off++] : 0) + 1);
    if (entries > 256) entries = 256;
    for (int i = 0; i < entries; ++i) {
      palette[i].red = (off < Size) ? Data[off++] : 0;
      palette[i].green = (off < Size) ? Data[off++] : 0;
      palette[i].blue = (off < Size) ? Data[off++] : 0;
    }
    png_set_PLTE(png_ptr, info_ptr, palette, entries);
  }

  png_color_16 background;
  memset(&background, 0, sizeof(background));
  background.index = (off < Size) ? Data[off++] : 0;
  background.red = ReadU16(Data, Size, &off);
  background.green = ReadU16(Data, Size, &off);
  background.blue = ReadU16(Data, Size, &off);
  background.gray = ReadU16(Data, Size, &off);
  png_set_bKGD(png_ptr, info_ptr, &background);

  int srgb_intent = (off < Size) ? (Data[off++] % 4) : 0;
  png_set_sRGB(png_ptr, info_ptr, srgb_intent);

  png_uint_32 res_x = ReadU32(Data, Size, &off);
  png_uint_32 res_y = ReadU32(Data, Size, &off);
  int unit_type = (off < Size) ? (Data[off++] % 3) : PNG_RESOLUTION_UNKNOWN;
  png_set_pHYs(png_ptr, info_ptr, res_x, res_y, unit_type);

  png_write_info(png_ptr, info_ptr);

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t bytes_per_sample = (bit_depth == 16) ? 2 : 1;
  size_t rowbytes = width * channels * bytes_per_sample;
  if (bit_depth < 8 && color_type != PNG_COLOR_TYPE_PALETTE && color_type != PNG_COLOR_TYPE_GRAY) {
    rowbytes = width * channels;
  }
  if (bit_depth < 8 && (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY)) {
    rowbytes = ((size_t)width * (size_t)bit_depth + 7) / 8;
  }

  png_bytep row = nullptr;
  if (rowbytes > 0) {
    row = (png_bytep)png_malloc(png_ptr, rowbytes);
    for (size_t y = 0; y < height; ++y) {
      for (size_t i = 0; i < rowbytes; ++i) {
        row[i] = (off < Size) ? Data[off++] : (uint8_t)(i + y);
      }
      png_write_row(png_ptr, row);
    }
    png_free(png_ptr, row);
  }

  png_write_end(png_ptr, info_ptr);

  fclose(fp);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  remove("./dummy_file");
  return 0;
}