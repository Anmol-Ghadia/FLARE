// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_rows at pngset.c:1546:1 in png.h
// png_write_png at pngwrite.c:1338:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_sRGB at pngset.c:653:1 in png.h
// png_set_gAMA_fixed at pngset.c:186:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
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

extern "C" int LLVMFuzzerTestOneInput_37(const uint8_t *Data, size_t Size) {
  FILE *seed_file = fopen("./dummy_file", "wb");
  if (seed_file != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, seed_file);
    fclose(seed_file);
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

  int width = (Size > 0 ? (int)(Data[0] % 32) + 1 : 1);
  int height = (Size > 1 ? (int)(Data[1] % 32) + 1 : 1);
  int color_type = (Size > 2 && (Data[2] & 1)) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
  int interlace = (Size > 3 && (Data[3] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  png_set_IHDR(png_ptr, info_ptr, width, height, 8, color_type,
               interlace, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

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

  png_fixed_point gamma = (png_fixed_point)(ReadU32(Data, Size, 11) % 1000000U);
  png_set_gAMA_fixed(png_ptr, info_ptr, gamma);

  const png_uint_32 masks[] = {
      PNG_INFO_gAMA,
      PNG_INFO_sRGB,
      PNG_INFO_tIME,
      PNG_INFO_PLTE,
      PNG_INFO_pHYs,
      PNG_INFO_oFFs
  };
  const size_t mask_count = sizeof(masks) / sizeof(masks[0]);
  png_uint_32 chosen_mask = masks[(Size > 15) ? (Data[15] % mask_count) : 0];

  (void)png_get_valid(png_ptr, info_ptr, chosen_mask);

  if (Size > 16 && (Data[16] & 1)) {
    png_set_invalid(png_ptr, info_ptr, (int)chosen_mask);
    (void)png_get_valid(png_ptr, info_ptr, chosen_mask);
  }

  int channels = (color_type == PNG_COLOR_TYPE_RGBA) ? 4 : 3;
  size_t rowbytes = (size_t)width * (size_t)channels;

  png_bytep *rows = (png_bytep *)malloc(sizeof(png_bytep) * (size_t)height);
  if (rows == NULL) {
    fclose(out);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_bytep image = (png_bytep)malloc((size_t)height * rowbytes);
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
#ifdef PNG_TRANSFORM_BGR
  if (Size > 17 && (Data[17] & 1)) transforms |= PNG_TRANSFORM_BGR;
#endif
#ifdef PNG_TRANSFORM_SWAP_ALPHA
  if (Size > 18 && (Data[18] & 1)) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
#endif
#ifdef PNG_TRANSFORM_INVERT_ALPHA
  if (Size > 19 && (Data[19] & 1)) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
#endif
#ifdef PNG_TRANSFORM_PACKING
  if (Size > 20 && (Data[20] & 1)) transforms |= PNG_TRANSFORM_PACKING;
#endif
#ifdef PNG_TRANSFORM_SHIFT
  if (Size > 21 && (Data[21] & 1)) transforms |= PNG_TRANSFORM_SHIFT;
#endif
#ifdef PNG_TRANSFORM_SWAP_ENDIAN
  if (Size > 22 && (Data[22] & 1)) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
#ifdef PNG_TRANSFORM_INVERT_MONO
  if (Size > 23 && (Data[23] & 1)) transforms |= PNG_TRANSFORM_INVERT_MONO;
#endif

  png_write_png(png_ptr, info_ptr, transforms, NULL);

  free(image);
  free(rows);
  fclose(out);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}