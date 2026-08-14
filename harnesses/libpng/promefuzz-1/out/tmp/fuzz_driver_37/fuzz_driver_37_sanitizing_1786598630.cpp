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
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return ((uint32_t)data[offset] << 24) |
         ((uint32_t)data[offset + 1] << 16) |
         ((uint32_t)data[offset + 2] << 8) |
         ((uint32_t)data[offset + 3]);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  FILE *out = fopen("./dummy_file", "wb");
  if (out == NULL) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_init_io(png_ptr, out);

  int width = (Size > 0 ? (Data[0] % 32) + 1 : 1);
  int height = (Size > 1 ? (Data[1] % 32) + 1 : 1);
  int color_type = (Size > 2 && (Data[2] & 1)) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
  int bit_depth = 8;
  int interlace = (Size > 3 && (Data[3] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression = PNG_COMPRESSION_TYPE_BASE;
  int filter = PNG_FILTER_TYPE_BASE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  png_time mod_time;
  memset(&mod_time, 0, sizeof(mod_time));
  mod_time.year = (png_uint_16)(Size > 4 ? 1900 + (Data[4] % 200) : 2000);
  mod_time.month = (png_byte)(Size > 5 ? Data[5] : 1);
  mod_time.day = (png_byte)(Size > 6 ? Data[6] : 1);
  mod_time.hour = (png_byte)(Size > 7 ? Data[7] : 0);
  mod_time.minute = (png_byte)(Size > 8 ? Data[8] : 0);
  mod_time.second = (png_byte)(Size > 9 ? Data[9] : 0);

  png_set_tIME(png_ptr, info_ptr, &mod_time);

  int srgb_intent = (Size > 10) ? (int)(Data[10] % 4) : PNG_sRGB_INTENT_PERCEPTUAL;
  png_set_sRGB(png_ptr, info_ptr, srgb_intent);

  png_fixed_point gamma = (png_fixed_point)(ReadU32(Data, Size, 11) % 1000000);
  png_set_gAMA_fixed(png_ptr, info_ptr, gamma);

  int masks[] = {
      PNG_INFO_gAMA, PNG_INFO_sRGB, PNG_INFO_tIME, PNG_INFO_PLTE,
      PNG_INFO_IDAT, PNG_INFO_IHDR, PNG_INFO_pHYs, PNG_INFO_oFFs
  };
  size_t mask_count = sizeof(masks) / sizeof(masks[0]);
  int chosen_mask = masks[(Size > 15) ? (Data[15] % mask_count) : 0];

  png_get_valid(png_ptr, info_ptr, (png_uint_32)chosen_mask);

  if (Size > 16 && (Data[16] & 1)) {
    png_set_invalid(png_ptr, info_ptr, chosen_mask);
    png_get_valid(png_ptr, info_ptr, (png_uint_32)chosen_mask);
  }

  png_bytep *rows = (png_bytep*)malloc(sizeof(png_bytep) * (size_t)height);
  if (rows == NULL) {
    fclose(out);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  int channels = (color_type == PNG_COLOR_TYPE_RGBA) ? 4 : 3;
  size_t rowbytes = (size_t)width * (size_t)channels;
  uint8_t *image = (uint8_t*)malloc((size_t)height * rowbytes);
  if (image == NULL) {
    free(rows);
    fclose(out);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  for (int y = 0; y < height; ++y) {
    rows[y] = image + (size_t)y * rowbytes;
  }

  for (size_t i = 0; i < (size_t)height * rowbytes; ++i) {
    image[i] = (Size > 0) ? Data[i % Size] : 0;
  }

  png_set_rows(png_ptr, info_ptr, rows);

  int transforms = 0;
  if (Size > 17 && (Data[17] & 1)) transforms |= PNG_TRANSFORM_BGR;
  if (Size > 18 && (Data[18] & 1)) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
  if (Size > 19 && (Data[19] & 1)) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
  if (Size > 20 && (Data[20] & 1)) transforms |= PNG_TRANSFORM_PACKING;
  if (Size > 21 && (Data[21] & 1)) transforms |= PNG_TRANSFORM_SHIFT;
  if (Size > 22 && (Data[22] & 1)) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
  if (Size > 23 && (Data[23] & 1)) transforms |= PNG_TRANSFORM_INVERT_MONO;

  png_write_png(png_ptr, info_ptr, transforms, NULL);

  free(image);
  free(rows);
  fclose(out);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}