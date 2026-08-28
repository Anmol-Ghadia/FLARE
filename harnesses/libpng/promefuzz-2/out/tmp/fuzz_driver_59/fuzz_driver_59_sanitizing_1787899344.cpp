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
#include <vector>

static void PngErrorFn(png_structp, png_const_charp)
{
  longjmp(png_jmpbuf(png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)), 1);
}

static void PngWarnFn(png_structp, png_const_charp)
{
}

static void WriteDummyFile(const uint8_t *Data, size_t Size)
{
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  WriteDummyFile(Data, Size);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_uint_32 width = 1;
  png_uint_32 height = 1;
  int bit_depth = 8;
  int color_type = PNG_COLOR_TYPE_RGB;
  int interlace_type = PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_method = PNG_FILTER_TYPE_BASE;

  if (Size >= 1)
    color_type = (Data[0] % 2) ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_GRAY;
  if (Size >= 2)
  {
    switch (Data[1] % 5)
    {
      case 0: bit_depth = 1; break;
      case 1: bit_depth = 2; break;
      case 2: bit_depth = 4; break;
      case 3: bit_depth = 8; break;
      default: bit_depth = 16; break;
    }
    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA && bit_depth < 8)
      bit_depth = 8;
  }
  if (Size >= 10)
  {
    width = ((png_uint_32)Data[2] << 8) | Data[3];
    height = ((png_uint_32)Data[4] << 8) | Data[5];
    if (width == 0) width = 1;
    if (height == 0) height = 1;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, compression_type, filter_method);

#ifdef PNG_gAMA_SUPPORTED
  if (Size >= 14)
  {
    png_fixed_point gamma_fixed =
        (png_fixed_point)(((uint32_t)Data[6] << 24) |
                          ((uint32_t)Data[7] << 16) |
                          ((uint32_t)Data[8] << 8) |
                          (uint32_t)Data[9]);
    png_set_gAMA_fixed(png_ptr, info_ptr, gamma_fixed);
  }
#endif

#ifdef PNG_sBIT_SUPPORTED
  if (Size >= 18)
  {
    png_color_8 sig_bit;
    memset(&sig_bit, 0, sizeof(sig_bit));
    sig_bit.red = Data[10];
    sig_bit.green = Data[11];
    sig_bit.blue = Data[12];
    sig_bit.gray = Data[13];
    sig_bit.alpha = Data[14];
    png_set_sBIT(png_ptr, info_ptr, &sig_bit);
  }
#endif

#ifdef PNG_sCAL_SUPPORTED
  if (Size >= 24)
  {
    int unit = (Data[15] % 2) ? PNG_SCALE_METER : PNG_SCALE_UNKNOWN;
    char width_str[32];
    char height_str[32];
    int a = (Size > 16) ? (int)Data[16] : 1;
    int b = (Size > 17) ? (int)Data[17] : 1;
    int c = (Size > 18) ? (int)Data[18] : 1;
    int d = (Size > 19) ? (int)Data[19] : 1;
    snprintf(width_str, sizeof(width_str), "%d.%d", a, b);
    snprintf(height_str, sizeof(height_str), "%d.%d", c, d);
    png_set_sCAL_s(png_ptr, info_ptr, unit, width_str, height_str);
  }
#endif

  int unit_out = 0;
  double width_out = 0.0, height_out = 0.0;
  png_fixed_point width_fixed = 0, height_fixed = 0;
  png_fixed_point gamma_out = 0;
  png_charp swidth = nullptr;
  png_charp sheight = nullptr;
  png_color_8p sig_bit_out = nullptr;

  png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_sBIT);
  png_get_valid(png_ptr, info_ptr, PNG_INFO_sCAL);
  png_get_valid(png_ptr, info_ptr, 0xffffffffU);

#ifdef PNG_gAMA_SUPPORTED
  png_get_gAMA_fixed(png_ptr, info_ptr, &gamma_out);
#endif

#ifdef PNG_sBIT_SUPPORTED
  png_get_sBIT(png_ptr, info_ptr, &sig_bit_out);
#endif

#ifdef PNG_sCAL_SUPPORTED
  png_get_sCAL(png_ptr, info_ptr, &unit_out, &width_out, &height_out);
  png_get_sCAL_fixed(png_ptr, info_ptr, &unit_out, &width_fixed, &height_fixed);
  png_get_sCAL_s(png_ptr, info_ptr, &unit_out, &swidth, &sheight);

  png_get_sCAL(png_ptr, info_ptr, nullptr, &width_out, &height_out);
  png_get_sCAL_fixed(png_ptr, info_ptr, &unit_out, nullptr, &height_fixed);
  png_get_sCAL_s(png_ptr, info_ptr, &unit_out, nullptr, &sheight);
#endif

  png_get_valid(nullptr, info_ptr, PNG_INFO_sCAL);
  png_get_valid(png_ptr, nullptr, PNG_INFO_sCAL);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}