// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_sBIT at pngset.c:638:1 in png.h
// png_set_sCAL_s at pngset.c:430:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_gAMA_fixed at pngget.c:678:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_get_sCAL at pngget.c:944:1 in png.h
// png_get_sCAL_fixed at pngget.c:921:1 in png.h
// png_get_sCAL_s at pngget.c:960:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_gAMA_fixed at pngset.c:186:1 in png.h
#include <png.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <setjmp.h>

static void WriteDummyFile(const uint8_t *Data, size_t Size)
{
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  WriteDummyFile(Data, Size);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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
  int color_type = PNG_COLOR_TYPE_RGB;
  int bit_depth = 8;

  if (Size > 0)
  {
    switch (Data[0] % 4)
    {
      case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
      case 1: color_type = PNG_COLOR_TYPE_RGB; break;
      case 2: color_type = PNG_COLOR_TYPE_PALETTE; break;
      default: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
    }
  }

  if (Size > 1)
  {
    switch (Data[1] % 5)
    {
      case 0: bit_depth = 1; break;
      case 1: bit_depth = 2; break;
      case 2: bit_depth = 4; break;
      case 3: bit_depth = 8; break;
      default: bit_depth = 16; break;
    }
  }

  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
  {
    if (bit_depth < 8) bit_depth = 8;
  }

  if (Size > 5)
  {
    width = ((png_uint_32)Data[2] << 8) | Data[3];
    height = ((png_uint_32)Data[4] << 8) | Data[5];
    if (width == 0) width = 1;
    if (height == 0) height = 1;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

#ifdef PNG_gAMA_SUPPORTED
  if (Size > 9)
  {
    png_fixed_point gamma_fixed =
        (png_fixed_point)(((png_uint_32)Data[6] << 24) |
                          ((png_uint_32)Data[7] << 16) |
                          ((png_uint_32)Data[8] << 8) |
                          (png_uint_32)Data[9]);
    png_set_gAMA_fixed(png_ptr, info_ptr, gamma_fixed);
  }
#endif

#ifdef PNG_sBIT_SUPPORTED
  if (Size > 14)
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
  if (Size > 19)
  {
    int unit = (Data[15] & 1) ? PNG_SCALE_METER : PNG_SCALE_UNKNOWN;
    char width_str[32];
    char height_str[32];
    snprintf(width_str, sizeof(width_str), "%u.%u",
             (unsigned)(Data[16] ? Data[16] : 1),
             (unsigned)Data[17]);
    snprintf(height_str, sizeof(height_str), "%u.%u",
             (unsigned)(Data[18] ? Data[18] : 1),
             (unsigned)Data[19]);
    png_set_sCAL_s(png_ptr, info_ptr, unit, width_str, height_str);
  }
#endif

  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_sBIT);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_sCAL);
  (void)png_get_valid(png_ptr, info_ptr, 0xffffffffU);
  (void)png_get_valid(nullptr, info_ptr, PNG_INFO_sCAL);
  (void)png_get_valid(png_ptr, nullptr, PNG_INFO_sCAL);

#ifdef PNG_gAMA_SUPPORTED
  png_fixed_point gamma_out = 0;
  (void)png_get_gAMA_fixed(png_ptr, info_ptr, &gamma_out);
#endif

#ifdef PNG_sBIT_SUPPORTED
  png_color_8p sig_bit_out = nullptr;
  (void)png_get_sBIT(png_ptr, info_ptr, &sig_bit_out);
#endif

#ifdef PNG_sCAL_SUPPORTED
  int unit_out = 0;
  double width_out = 0.0, height_out = 0.0;
  png_fixed_point width_fixed = 0, height_fixed = 0;
  png_charp swidth = nullptr;
  png_charp sheight = nullptr;

  (void)png_get_sCAL(png_ptr, info_ptr, &unit_out, &width_out, &height_out);
  (void)png_get_sCAL_fixed(png_ptr, info_ptr, &unit_out, &width_fixed, &height_fixed);
  (void)png_get_sCAL_s(png_ptr, info_ptr, &unit_out, &swidth, &sheight);
#endif

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}