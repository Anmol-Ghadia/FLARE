// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_cHRM_XYZ_fixed at pngset.c:69:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_get_sRGB at pngget.c:718:1 in png.h
// png_get_sRGB at pngget.c:718:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
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
#include <string.h>
#include <setjmp.h>

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t *offset) {
  png_uint_32 v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (*offset < size) v |= data[(*offset)++];
  }
  return v;
}

static png_int_32 ReadS32(const uint8_t *data, size_t size, size_t *offset) {
  return (png_int_32)ReadU32(data, size, offset);
}

static double ReadDoubleLike(const uint8_t *data, size_t size, size_t *offset) {
  png_int_32 s = ReadS32(data, size, offset);
  int mode = 0;
  if (*offset < size) mode = data[(*offset)++] & 3;
  switch (mode) {
    case 0: return (double)s / 100000.0;
    case 1: return (double)s / 1000.0;
    case 2: return (double)s;
    default: return (double)(s % 1000000) / 1000000.0;
  }
}

static void WriteDataFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE *fp = (FILE*)png_get_io_ptr(png_ptr);
  if (fp != NULL) fwrite(data, 1, length, fp);
}

static void FlushDataFn(png_structp png_ptr) {
  FILE *fp = (FILE*)png_get_io_ptr(png_ptr);
  if (fp != NULL) fflush(fp);
}

extern "C" int LLVMFuzzerTestOneInput_73(const uint8_t *Data, size_t Size) {
  FILE *dummy = fopen("./dummy_file", "wb");
  if (dummy != NULL) {
    if (Size) fwrite(Data, 1, Size, dummy);
    fclose(dummy);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
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

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    png_set_write_fn(png_ptr, fp, WriteDataFn, FlushDataFn);

    png_set_IHDR(png_ptr, info_ptr,
                 1, 1, 8,
                 PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    size_t off = 0;

    png_fixed_point wx = ReadS32(Data, Size, &off);
    png_fixed_point wy = ReadS32(Data, Size, &off);
    png_fixed_point rx = ReadS32(Data, Size, &off);
    png_fixed_point ry = ReadS32(Data, Size, &off);
    png_fixed_point gx = ReadS32(Data, Size, &off);
    png_fixed_point gy = ReadS32(Data, Size, &off);
    png_fixed_point bx = ReadS32(Data, Size, &off);
    png_fixed_point by = ReadS32(Data, Size, &off);

    double dwx = ReadDoubleLike(Data, Size, &off);
    double dwy = ReadDoubleLike(Data, Size, &off);
    double drx = ReadDoubleLike(Data, Size, &off);
    double dry = ReadDoubleLike(Data, Size, &off);
    double dgx = ReadDoubleLike(Data, Size, &off);
    double dgy = ReadDoubleLike(Data, Size, &off);
    double dbx = ReadDoubleLike(Data, Size, &off);
    double dby = ReadDoubleLike(Data, Size, &off);

    png_fixed_point rX = ReadS32(Data, Size, &off);
    png_fixed_point rY = ReadS32(Data, Size, &off);
    png_fixed_point rZ = ReadS32(Data, Size, &off);
    png_fixed_point gX = ReadS32(Data, Size, &off);
    png_fixed_point gY = ReadS32(Data, Size, &off);
    png_fixed_point gZ = ReadS32(Data, Size, &off);
    png_fixed_point bX = ReadS32(Data, Size, &off);
    png_fixed_point bY = ReadS32(Data, Size, &off);
    png_fixed_point bZ = ReadS32(Data, Size, &off);

    int srgb_intent = 0;
    if (off < Size) srgb_intent = (int)((signed char)Data[off++]);

    if (off < Size) {
      switch (Data[off++] % 6) {
        case 0:
          png_set_cHRM_fixed(png_ptr, info_ptr, wx, wy, rx, ry, gx, gy, bx, by);
          break;
        case 1:
          png_set_cHRM(png_ptr, info_ptr, dwx, dwy, drx, dry, dgx, dgy, dbx, dby);
          break;
        case 2:
          png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);
          break;
        case 3:
          png_set_cHRM_fixed(png_ptr, info_ptr, wx, wy, rx, ry, gx, gy, bx, by);
          png_set_cHRM(png_ptr, info_ptr, dwx, dwy, drx, dry, dgx, dgy, dbx, dby);
          break;
        case 4:
          png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);
          png_set_cHRM_fixed(png_ptr, info_ptr, wx, wy, rx, ry, gx, gy, bx, by);
          break;
        default:
          png_set_cHRM(png_ptr, info_ptr, dwx, dwy, drx, dry, dgx, dgy, dbx, dby);
          png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);
          break;
      }
    }

    png_set_cHRM_fixed(png_ptr, info_ptr, wx, wy, rx, ry, gx, gy, bx, by);
    png_set_cHRM(png_ptr, info_ptr, dwx, dwy, drx, dry, dgx, dgy, dbx, dby);
    png_set_cHRM_XYZ_fixed(png_ptr, info_ptr, rX, rY, rZ, gX, gY, gZ, bX, bY, bZ);
    png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);

    double o_wx = 0, o_wy = 0, o_rx = 0, o_ry = 0, o_gx = 0, o_gy = 0, o_bx = 0, o_by = 0;
    png_get_cHRM(png_ptr, info_ptr, &o_wx, &o_wy, &o_rx, &o_ry, &o_gx, &o_gy, &o_bx, &o_by);
    png_get_cHRM(png_ptr, info_ptr, NULL, &o_wy, NULL, &o_ry, NULL, &o_gy, NULL, &o_by);
    png_get_cHRM(png_ptr, info_ptr, &o_wx, NULL, &o_rx, NULL, &o_gx, NULL, &o_bx, NULL);

    png_fixed_point orX = 0, orY = 0, orZ = 0, ogX = 0, ogY = 0, ogZ = 0, obX = 0, obY = 0, obZ = 0;
    png_get_cHRM_XYZ_fixed(png_ptr, info_ptr, &orX, &orY, &orZ, &ogX, &ogY, &ogZ, &obX, &obY, &obZ);
    png_get_cHRM_XYZ_fixed(png_ptr, info_ptr, NULL, &orY, NULL, &ogX, NULL, &ogZ, NULL, &obY, NULL);

    int out_intent = 0;
    png_get_sRGB(png_ptr, info_ptr, &out_intent);
    png_get_sRGB(png_ptr, info_ptr, NULL);

    png_write_info(png_ptr, info_ptr);
    png_byte row[3] = {0, 0, 0};
    png_write_row(png_ptr, row);
    png_write_end(png_ptr, info_ptr);

    fclose(fp);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}