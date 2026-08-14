// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_PLTE at pngset.c:572:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_set_benign_errors at pngset.c:1671:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_gamma_fixed at pngrtran.c:818:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_rows at pngset.c:1546:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_write_png at pngwrite.c:1338:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = (FILE*)png_get_io_ptr(png_ptr);
  if (fp != NULL && data != NULL && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = (FILE*)png_get_io_ptr(png_ptr);
  if (fp != NULL) {
    fflush(fp);
  }
}

static size_t ComputeInputRowBytes(png_uint_32 width, int color_type, int bit_depth) {
  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  if (bit_depth < 8) {
    return ((size_t)width * (size_t)channels * (size_t)bit_depth + 7u) / 8u;
  }

  return (size_t)width * (size_t)channels * (size_t)(bit_depth / 8);
}

static int PickColorType(uint8_t v) {
  switch (v % 5) {
    case 0: return PNG_COLOR_TYPE_GRAY;
    case 1: return PNG_COLOR_TYPE_GRAY_ALPHA;
    case 2: return PNG_COLOR_TYPE_RGB;
    case 3: return PNG_COLOR_TYPE_RGB_ALPHA;
    default: return PNG_COLOR_TYPE_PALETTE;
  }
}

static int PickBitDepth(uint8_t v) {
  static const int kDepths[] = {1, 2, 4, 8, 16};
  return kDepths[v % 5];
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    remove("./dummy_file");
    return 0;
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == NULL) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  png_uint_32 width = Size > 0 ? (png_uint_32)(Data[0] % 8u + 1u) : 1u;
  png_uint_32 height = Size > 1 ? (png_uint_32)(Data[1] % 8u + 1u) : 1u;
  int color_type = PickColorType(Size > 2 ? Data[2] : 0);
  int bit_depth = PickBitDepth(Size > 3 ? Data[3] : 0);

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    if (bit_depth < 8) bit_depth = 8;
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
    bit_depth = 8;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_size = (bit_depth == 1) ? 2 : (bit_depth == 2) ? 4 : (bit_depth == 4) ? 16 : 256;
    for (int i = 0; i < palette_size; ++i) {
      uint8_t v = (Size > (size_t)(4 + i)) ? Data[4 + i] : (uint8_t)i;
      palette[i].red = v;
      palette[i].green = (png_byte)(v ^ 0x55u);
      palette[i].blue = (png_byte)(v ^ 0xAAu);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_size);
  }

  png_set_benign_errors(png_ptr, Size > 4 ? (int)(Data[4] & 1u) : 0);
  png_set_bgr(png_ptr);
  png_set_packing(png_ptr);

  int flush_rows = 0;
  if (Size > 5) {
    flush_rows = (int)(int8_t)Data[5];
  }
  png_set_flush(png_ptr, flush_rows);

  png_fixed_point screen_gamma = 100000;
  png_fixed_point file_gamma = 100000;
  if (Size > 9) {
    screen_gamma = (png_fixed_point)(
        ((png_fixed_point)Data[6] << 24) |
        ((png_fixed_point)Data[7] << 16) |
        ((png_fixed_point)Data[8] << 8) |
        (png_fixed_point)Data[9]);
  }
  if (Size > 13) {
    file_gamma = (png_fixed_point)(
        ((png_fixed_point)Data[10] << 24) |
        ((png_fixed_point)Data[11] << 16) |
        ((png_fixed_point)Data[12] << 8) |
        (png_fixed_point)Data[13]);
  }
  png_set_gamma_fixed(png_ptr, screen_gamma, file_gamma);

  size_t rowbytes = ComputeInputRowBytes(width, color_type, bit_depth);
  if (rowbytes == 0) rowbytes = 1;

  png_bytep image = (png_bytep)malloc(rowbytes * (size_t)height);
  png_bytepp rows = (png_bytepp)malloc(sizeof(png_bytep) * (size_t)height);
  if (image == NULL || rows == NULL) {
    free(image);
    free(rows);
    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    remove("./dummy_file");
    return 0;
  }

  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image + (size_t)y * rowbytes;
    for (size_t x = 0; x < rowbytes; ++x) {
      size_t idx = 14u + (size_t)y * rowbytes + x;
      rows[y][x] = Size > idx ? Data[idx] : (uint8_t)((x + y) & 0xFFu);
    }
  }

  png_set_rows(png_ptr, info_ptr, rows);

  int transforms = 0;
  if (Size > 14) {
    if (Data[14] & 0x01u) transforms |= PNG_TRANSFORM_BGR;
    if (Data[14] & 0x02u) transforms |= PNG_TRANSFORM_PACKING;
    if (Data[14] & 0x04u) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
    if (Data[14] & 0x08u) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
    if (Data[14] & 0x10u) transforms |= PNG_TRANSFORM_STRIP_FILLER_AFTER;
    if (Data[14] & 0x20u) transforms |= PNG_TRANSFORM_STRIP_FILLER_BEFORE;
    if (Data[14] & 0x40u) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
    if (Data[14] & 0x80u) transforms |= PNG_TRANSFORM_INVERT_MONO;
  }

  if (bit_depth < 8) {
    transforms &= ~PNG_TRANSFORM_PACKING;
  }

  if (bit_depth != 16) {
    transforms &= ~PNG_TRANSFORM_SWAP_ENDIAN;
  }

  if (!(color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA)) {
    transforms &= ~PNG_TRANSFORM_BGR;
  }

  if (!(color_type == PNG_COLOR_TYPE_GRAY_ALPHA || color_type == PNG_COLOR_TYPE_RGB_ALPHA)) {
    transforms &= ~PNG_TRANSFORM_INVERT_ALPHA;
    transforms &= ~PNG_TRANSFORM_SWAP_ALPHA;
    transforms &= ~PNG_TRANSFORM_STRIP_FILLER_AFTER;
    transforms &= ~PNG_TRANSFORM_STRIP_FILLER_BEFORE;
  }

  png_write_png(png_ptr, info_ptr, transforms, NULL);

  free(image);
  free(rows);
  fclose(fp);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  remove("./dummy_file");
  return 0;
}