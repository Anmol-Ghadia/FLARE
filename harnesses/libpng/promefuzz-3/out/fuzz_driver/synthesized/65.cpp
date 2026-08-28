// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_sCAL_fixed at pngset.c:527:1 in png.h
// png_set_sCAL at pngset.c:497:1 in png.h
// png_set_sCAL_s at pngset.c:430:1 in png.h
// png_set_sCAL_fixed at pngset.c:527:1 in png.h
// png_set_sCAL at pngset.c:497:1 in png.h
// png_set_sCAL_s at pngset.c:430:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
// png_set_sCAL_s at pngset.c:430:1 in png.h
// png_set_sCAL at pngset.c:497:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
// png_set_sCAL_fixed at pngset.c:527:1 in png.h
// png_get_sCAL at pngget.c:944:1 in png.h
// png_get_sCAL_fixed at pngget.c:921:1 in png.h
// png_get_sCAL_s at pngget.c:960:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
// png_get_sCAL at pngget.c:944:1 in png.h
// png_get_sCAL_s at pngget.c:960:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
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
#include <setjmp.h>
#include <stdio.h>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t *off) {
  if (*off + 4 > size) return 0;
  uint32_t v = ((uint32_t)data[*off] << 24) |
               ((uint32_t)data[*off + 1] << 16) |
               ((uint32_t)data[*off + 2] << 8) |
               (uint32_t)data[*off + 3];
  *off += 4;
  return v;
}

static int32_t ReadS32(const uint8_t *data, size_t size, size_t *off) {
  return (int32_t)ReadU32(data, size, off);
}

static double ReadDoubleLike(const uint8_t *data, size_t size, size_t *off) {
  uint64_t hi = ReadU32(data, size, off);
  uint64_t lo = ReadU32(data, size, off);
  uint64_t bits = (hi << 32) | lo;
  double d;
  memcpy(&d, &bits, sizeof(d));
  return d;
}

static char *MakeString(const uint8_t *data, size_t size, size_t *off) {
  if (*off >= size) {
    char *s = (char*)malloc(2);
    if (!s) return NULL;
    s[0] = '1';
    s[1] = '\0';
    return s;
  }

  size_t remaining = size - *off;
  size_t len = data[*off] % 32;
  (*off)++;
  if (len > remaining - 1) len = remaining - 1;

  char *s = (char*)malloc(len + 1);
  if (!s) return NULL;

  for (size_t i = 0; i < len; ++i) {
    unsigned char c = data[*off + i];
    if (c == 0) c = '0';
    s[i] = (char)c;
  }
  s[len] = '\0';
  *off += len;

  if (len == 0) {
    free(s);
    s = (char*)malloc(2);
    if (!s) return NULL;
    s[0] = '1';
    s[1] = '\0';
  }

  return s;
}

static void WriteDummyFile(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_65(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  size_t off = 0;

  int unit1 = (int)(ReadU32(Data, Size, &off) % 4);
  int unit2 = (int)(ReadU32(Data, Size, &off) % 4);
  int unit3 = (int)(ReadU32(Data, Size, &off) % 4);

  png_fixed_point fw = (png_fixed_point)ReadS32(Data, Size, &off);
  png_fixed_point fh = (png_fixed_point)ReadS32(Data, Size, &off);

  double dw = ReadDoubleLike(Data, Size, &off);
  double dh = ReadDoubleLike(Data, Size, &off);

  char *sw = MakeString(Data, Size, &off);
  char *sh = MakeString(Data, Size, &off);
  char *sw2 = MakeString(Data, Size, &off);
  char *sh2 = MakeString(Data, Size, &off);

  if (!sw || !sh || !sw2 || !sh2) {
    free(sw);
    free(sh);
    free(sw2);
    free(sh2);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  switch (Size ? (Data[0] % 6) : 0) {
    case 0:
      png_set_sCAL_fixed(png_ptr, info_ptr, unit1, fw, fh);
      break;
    case 1:
      png_set_sCAL(png_ptr, info_ptr, unit2, dw, dh);
      break;
    case 2:
      png_set_sCAL_s(png_ptr, info_ptr, unit3, sw, sh);
      break;
    case 3:
      png_set_sCAL_fixed(png_ptr, info_ptr, 1, fw > 0 ? fw : 100000, fh > 0 ? fh : 100000);
      png_set_sCAL(png_ptr, info_ptr, 2, dw, dh);
      break;
    case 4:
      png_set_sCAL_s(png_ptr, info_ptr, 1 + (unit1 & 1), sw, sh);
      png_set_invalid(png_ptr, info_ptr, PNG_INFO_sCAL);
      png_set_sCAL_s(png_ptr, info_ptr, 1 + (unit2 & 1), sw2, sh2);
      break;
    case 5:
      png_set_sCAL(png_ptr, info_ptr, 1 + (unit1 & 1), dw, dh);
      png_set_invalid(png_ptr, info_ptr, PNG_INFO_sCAL);
      png_set_sCAL_fixed(png_ptr, info_ptr, 1 + (unit2 & 1),
                         fw > 0 ? fw : 1, fh > 0 ? fh : 1);
      break;
  }

  int out_unit = 0;
  double out_w = 0.0, out_h = 0.0;
  png_fixed_point out_fw = 0, out_fh = 0;
  png_charp out_sw = NULL;
  png_charp out_sh = NULL;

  (void)png_get_sCAL(png_ptr, info_ptr, &out_unit, &out_w, &out_h);
#if defined(PNG_FLOATING_ARITHMETIC_SUPPORTED) || defined(PNG_FLOATING_POINT_SUPPORTED)
  (void)png_get_sCAL_fixed(png_ptr, info_ptr, &out_unit, &out_fw, &out_fh);
#endif
  (void)png_get_sCAL_s(png_ptr, info_ptr, &out_unit, &out_sw, &out_sh);

  if (Size > 1) {
    png_set_invalid(png_ptr, info_ptr, (int)ReadU32(Data, Size, &off));
    (void)png_get_sCAL(png_ptr, info_ptr, &out_unit, &out_w, &out_h);
    (void)png_get_sCAL_s(png_ptr, info_ptr, &out_unit, &out_sw, &out_sh);
  }

  free(sw);
  free(sh);
  free(sw2);
  free(sh2);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}